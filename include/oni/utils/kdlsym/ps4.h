#pragma once
#include <oni/config.h>
#if ONI_PLATFORM==ONI_PS4_PLATFORM

/*
Приветствие zeco, specter
These are the required functions in order for the Oni Framework to operate properly
These are all offsets into the base of the kernel. They expect all standard FreeBSD 9 prototypes

The reason we do not hardcode offsets here, is due to the different platforms that are supported, and
for the platforms that do enable kernel ASLR (Address Space Layout Randomization?)
*/

#define kdlsym_addr_allproc					0x00142A40
#define kdlsym_addr_allproc_lock			0x01AD76B8

#define kdlsym_addr_critical_enter			0x0023D560
#define kdlsym_addr_critical_exit			0x0023D570

#define kdlsym_addr_kernel_map				0x01B31218
#define kdlsym_addr_kern_reboot				0x000998A0
#define kdlsym_addr_kmem_alloc				0x0016ECD0
#define kdlsym_addr_kmem_free				0x0016EEA0
#define kdlsym_addr_kproc_create			0x00464700

#define kdlsym_addr_sys_mlock				0x00142A40

#define kdlsym_addr_pfind					0x00079780
#define kdlsym_addr_printf					0x00017F30
#define kdlsym_addr_proc_rwmem				0x0017CB70
#define kdlsym_addr_pmap_activate			0x00427530

#define kdlsym_addr_vmspace_alloc			0x00391A70
#define kdlsym_addr_vmspace_free			0x00391D10

#define kdlsym_addr_snprintf				0x00018230

#define kdlsym_addr_vsnprintf				0x000182D0

#define kdlsym_addr__mtx_lock_flags			0x0030D6A0
#define kdlsym_addr__mtx_unlock_flags		0x0030D940

#define kdlsym_addr__sx_slock				0x0038F980
#define kdlsym_addr__sx_sunlock				0x0038FB00

#define kdlsym_addr_vmspace_acquire_ref		0x00391EE0
#define kdlsym_addr__vm_map_lock_read		0x003920B0
#define kdlsym_addr__vm_map_unlock_read		0x00392100

// TODO: Sort by alpha

#define kdlsym_addr_sys_lseek				0x00441D20
#define kdlsym_addr_sys_mmap				0x001419F0
#define kdlsym_addr_sys_munmap				0x00142190
#define kdlsym_addr_utilUSleep				0x0065F290
#define kdlsym_addr_kthread_exit			0x00464F60
#define kdlsym_addr_kthread_add				0x00464C90
#define kdlsym_addr_sys_read				0x0005EC50
#define kdlsym_addr_sys_fstat				0x0042B1B0
#define kdlsym_addr_sys_close				0x0042AC00
#define kdlsym_addr_sys_socket				0x003EAC20
#define kdlsym_addr_sys_write				0x0005F1A0


#define kdlsym_addr_sys_getdents			0x00445600
#define kdlsym_addr_sys_bind				0x003EB550
#define kdlsym_addr_sys_listen				0x003EB760
#define kdlsym_addr_sys_accept				0x003EBEC0
#define kdlsym_addr_sys_recvfrom			0x003ED080
#define kdlsym_addr_sys_sendto				0x003EC9C0
#define kdlsym_addr_sys_open				0x0043FCE0
#define kdlsym_addr_memcpy					0x0014A6B0
#define kdlsym_addr_memset					0x00302BD0
#define kdlsym_addr_sys_stat				0x00442320

#define	kdlsym_addr_Xfast_syscall           0x003095D0
#define kdlsym_addr_sys_dup2				0x00428E50
#define kdlsym_addr_sys_shutdown			0x003ED250
#define kdlsym_addr_sys_unlink				0x00441730
#define kdlsym_addr_sys_setuid				0x00114450

#define kdlsym_addr_self_orbis_sysvec 0x01460610
#endif
