#include <oni/utils/memory/install.h>
#include <oni/utils/kdlsym.h>
#include <oni/utils/kernel.h>
#include <oni/init/initparams.h>
#include <oni/utils/sys_wrappers.h>
#include <oni/utils/memory/allocator.h>
#include <oni/utils/patches.h>

#include <sys/types.h>
#include <sys/unistd.h>
#include <sys/proc.h>
#include <sys/kthread.h>
#include <sys/imgact.h>
#include <vm/vm.h>
#include <vm/vm_page.h>
#include <vm/pmap.h>
#include <vm/vm_map.h>
#include <vm/vm_param.h>
#include <unistd.h>

#include <oni/utils/cpu.h>
#include <oni/utils/syscall.h>

uint8_t* gUserBaseAddress = NULL;
uint32_t gUserBaseSize = 0;
void* gElevatedEntryPoint = NULL;

void SelfElevateAndRunStage2();



uint8_t SelfElevateAndRun(uint8_t* userlandPayload, uint32_t userlandSize, void(*elevatedEntryPoint)(void* arguments))
{
	// Verify arguments
	if (!userlandPayload || !userlandSize || !elevatedEntryPoint)
		return false;

	// Assign the user base address and size
	gUserBaseAddress = userlandPayload;
	gUserBaseSize = userlandSize;
	gElevatedEntryPoint = elevatedEntryPoint;

	// TODO: kexec
	syscall1(11, SelfElevateAndRunStage2);

	return true;
}

void SelfElevateAndRunStage2()
{
	// Fill the kernel base address
	gKernelBase = (uint8_t*)kernelRdmsr(0xC0000082) - kdlsym_addr_Xfast_syscall;

	//0x40000;
	void(*critical_enter)(void) = kdlsym(critical_enter);
	void(*crtical_exit)(void) = kdlsym(critical_exit);
	vm_offset_t(*kmem_alloc)(vm_map_t map, vm_size_t size) = kdlsym(kmem_alloc);
	void(*printf)(char *format, ...) = kdlsym(printf);
	int(*kproc_create)(void(*func)(void*), void* arg, struct proc** newpp, int flags, int pages, const char* fmt, ...) = kdlsym(kproc_create);
	vm_map_t map = (vm_map_t)(*(uint64_t *)(kdlsym(kernel_map)));
	void* (*memset)(void *s, int c, size_t n) = kdlsym(memset);
	void* (*memcpy)(void* dest, const void* src, size_t n) = kdlsym(memcpy);

	// Apply patches
	critical_enter();
	cpu_disable_wp();

	oni_installPrePatches();

	cpu_enable_wp();
	crtical_exit();

	// We currently are in kernel context, executing userland memory
	if (!gUserBaseAddress || !gUserBaseSize || !gElevatedEntryPoint)
		return;

	printf("[*] Got userland payload %p %x\n", gUserBaseAddress, gUserBaseSize);

	// Allocate some memory
	uint8_t* kernelPayload = (uint8_t*)kmem_alloc(map, gUserBaseSize);
	if (!kernelPayload)
		return;

	printf("[+] Allocated kernel executable memory %p\n", kernelPayload);

	// Find the exported init_kernelStartup symbol
	uint64_t kernelStartupSlide = (uint64_t)gElevatedEntryPoint - (uint64_t)gUserBaseAddress;
	printf("[*] Kernel startup slide %x\n", kernelStartupSlide);

	uint8_t* kernelStartup = kernelPayload + kernelStartupSlide;
	if (!kernelStartup)
		return;

	printf("[+] Pre-faulting user memory %p %x\n", 0x926100000, 0x200000);
	kmlockall(1);

	printf("[+] Pre-faulting kernel memory %p %x\n", kernelPayload, gUserBaseSize);
	kmlock((void*)kernelPayload, gUserBaseSize);

	printf("[*] Kernel startup address %p\n", kernelStartup);

	// Create launch parameters, this is floating in "free kernel space" so the other process should
	// be able to grab and use the pointer directly
	struct initparams_t* initParams = (struct initparams_t*)kmem_alloc(map, sizeof(struct initparams_t));
	if (!initParams)
	{
		printf("[-] Could not allocate new init params\n");
		return;
	}

	initParams->payloadBase = (uint64_t)kernelPayload;
	initParams->payloadSize = gUserBaseSize;
	initParams->process = 0;

	printf("[+] Copying payload from user-land\n");
	memset(kernelPayload, 0, gUserBaseSize);
	memcpy(kernelPayload, gUserBaseAddress, gUserBaseSize);

	printf("[*] Kernel payload peek: %02X %02X %02X %02X %02X\n", kernelPayload[0], kernelPayload[1], kernelPayload[2], kernelPayload[3], kernelPayload[4]);
	printf("[*] Kernel oni_kernel_startup peek: %02X %02X %02X %02X %02X\n", kernelStartup[0], kernelStartup[1], kernelStartup[2], kernelStartup[3], kernelStartup[4]);

	// Create new process
	printf("[+] Calling kproc_create(%p, %p, %p, %d, %d, %s);\n", kernelStartup, initParams, &initParams->process, 0, 0, "install");

	critical_enter();
	int processCreateResult = kproc_create((void(*)(void*))kernelStartup, initParams, &initParams->process, 0, 0, "install");
	crtical_exit();

	if (processCreateResult != 0)
		printf("[-] Failed to create process.\n");
	else
		printf("[+] Kernel process created. Result %d\n", processCreateResult);
}