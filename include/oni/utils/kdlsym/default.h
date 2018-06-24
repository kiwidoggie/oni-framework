#pragma once
#include <oni/config.h>
#if ONI_PLATFORM==ONI_UNKNOWN_PLATFORM

/*
These are the required functions in order for the Oni Framework to operate properly
These are all offsets into the base of the kernel. They expect all standard FreeBSD 9 prototypes

The reason we do not hardcode offsets here, is due to the different platforms that are supported, and
for the platforms that do enable kernel ASLR (Address Space Layout Randomization?)
*/

#define kdlsym_addr_allproc					0xDEADC0DE
#define kdlsym_addr_allproc_lock			0xDEADC0DE

#define kdlsym_addr_critical_enter			0xDEADC0DE
#define kdlsym_addr_critical_exit			0xDEADC0DE

#define kdlsym_addr_kernel_map				0xDEADC0DE
#define kdlsym_addr_kern_reboot				0xDEADC0DE
#define kdlsym_addr_kmem_alloc				0xDEADC0DE
#define kdlsym_addr_kmem_free				0xDEADC0DE
#define kdlsym_addr_kproc_create			0xDEADC0DE

#define kdlsym_addr_sys_mlock				0xDEADC0DE
#define kdlsym_addr_sys_mlockall			0xDEADC0DE

#define kdlsym_addr_pfind					0xDEADC0DE
#define kdlsym_addr_printf					0xDEADC0DE
#define kdlsym_addr_proc_rwmem				0xDEADC0DE
#define kdlsym_addr_pmap_activate			0xDEADC0DE

#define kdlsym_addr_vmspace_alloc			0xDEADC0DE
#define kdlsym_addr_vmspace_free			0xDEADC0DE

#define kdlsym_addr_snprintf				0xDEADC0DE

#define kdlsym_addr_vsnprintf				0xDEADC0DE

#define kdlsym_addr__mtx_lock_flags			0xDEADC0DE
#define kdlsym_addr__mtx_unlock_flags		0xDEADC0DE

#define kdlsym_addr__sx_slock				0xDEADC0DE
#define kdlsym_addr__sx_sunlock				0xDEADC0DE

#define kdlsym_addr_vmspace_acquire_ref		0xDEADC0DE
#define kdlsym_addr__vm_map_lock_read		0xDEADC0DE
#define kdlsym_addr__vm_map_unlock_read		0xDEADC0DE

// TODO: Sort by alpha

#define kdlsym_addr_sys_lseek				0xDEADC0DE
#define kdlsym_addr_sys_mmap				0xDEADC0DE
#define kdlsym_addr_sys_munmap				0xDEADC0DE
#define kdlsym_addr_utilUSleep				0xDEADC0DE
#define kdlsym_addr_kthread_exit			0xDEADC0DE
#define kdlsym_addr_kthread_add				0xDEADC0DE
#define kdlsym_addr_sys_read				0xDEADC0DE
#define kdlsym_addr_sys_fstat				0xDEADC0DE
#define kdlsym_addr_sys_close				0xDEADC0DE
#define kdlsym_addr_sys_socket				0xDEADC0DE
#define kdlsym_addr_sys_write				0xDEADC0DE


#define kdlsym_addr_sys_getdents			0xDEADC0DE
#define kdlsym_addr_sys_bind				0xDEADC0DE
#define kdlsym_addr_sys_listen				0xDEADC0DE
#define kdlsym_addr_sys_accept				0xDEADC0DE
#define kdlsym_addr_sys_recvfrom			0xDEADC0DE
#define kdlsym_addr_sys_sendto				0xDEADC0DE
#define kdlsym_addr_sys_open				0xDEADC0DE
#define kdlsym_addr_memcpy					0xDEADC0DE
#define kdlsym_addr_memset					0xDEADC0DE
#define kdlsym_addr_sys_stat				0xDEADC0DE

#define	kdlsym_addr_Xfast_syscall           0xDEADC0DE
#define kdlsym_addr_sys_dup2				0xDEADC0DE
#define kdlsym_addr_sys_shutdown			0xDEADC0DE
#define kdlsym_addr_sys_unlink				0xDEADC0DE
#define kdlsym_addr_sys_setuid				0xDEADC0DE
#define kdlsym_addr_sys_ptrace				0xDEADC0DE
#define kdlsym_addr_sscanf					0xDEADC0DE
#define kdlsym_addr_mtx_init				0xDEADC0DE
#define kdlsym_addr_sys_mlock				0xDEADC0DE
#define kdlsym_addr_sys_kill				0xDEADC0DE
#define kdlsym_addr_sys_mkdir				0xDEADC0DE
#define kdlsym_addr_sys_rmdir				0xDEADC0DE

#define kdlsym_addr_eventhandler_register	0xDEADC0DE
#define kdlsym_addr_prison0					0xDEADC0DE
#define kdlsym_addr_rootvnode				0xDEADC0DE
#define	kdlsym_addr_copyin					0xDEADC0DE
#define kdlsym_addr_self_orbis_sysvec		0xDEADC0DE
#define kdlsym_addr_sys_wait4				0xDEADC0DE
#define	kdlsym_addr_memcmp                  0xDEADC0DE
#endif