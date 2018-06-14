#pragma once

#if ONI_PLATFORM==ONI_PLATFORM_ORBIS_BSD_505
/*
These are the required functions in order for the Oni Framework to operate properly
These are all offsets into the base of the kernel. They expect all standard FreeBSD 9 prototypes

The reason we do not hardcode offsets here, is due to the different platforms that are supported, and
for the platforms that do enable kernel ASLR (Address Space Layout Randomization?)
*/
#define kdlsym_addr_allproc					0x2382ff8
#define kdlsym_addr_allproc_lock			0x2382f98

#define	kdlsym_addr_critical_enter          0x28e7a0
#define	kdlsym_addr_critical_exit           0x28e7b0

#define kdlsym_addr_kernel_map				0x1AC60E0
#define kdlsym_addr_kern_reboot				0x0010D390
#define	kdlsym_addr_kmem_alloc              0xfcc80
#define	kdlsym_addr_kmem_free               0xfce50
#define	kdlsym_addr_kproc_create			0x137df0

#define kdlsym_addr_sys_mlock				0x0013E250
#define kdlsym_addr_sys_mlockall			0x0013E300

#define	kdlsym_addr_pfind                   0x4034e0
#define	kdlsym_addr_printf                  0x436040
#define	kdlsym_addr_proc_rwmem              0x30d150
#define	kdlsym_addr_pmap_activate           0x2eafd0

#define	kdlsym_addr_vmspace_alloc           0x19eb20
#define	kdlsym_addr_vmspace_free            0x19edc0

#define	kdlsym_addr_snprintf                0x436350

#define kdlsym_addr_vsnprintf				0x004363F0

#define kdlsym_addr__mtx_lock_flags			0x401cd0
#define kdlsym_addr__mtx_unlock_flags		0x401fa0

#define	kdlsym_addr__sx_slock               0xf5c30
#define	kdlsym_addr__sx_xlock               0xf5e10
#define	kdlsym_addr__sx_sunlock             0xf5f10
#define	kdlsym_addr__sx_xunlock	            0xf5fd0

#define kdlsym_addr_vmspace_acquire_ref		0x19ef90
#define kdlsym_addr__vm_map_lock_read		0x19f140
#define kdlsym_addr__vm_map_unlock_read		0x19f190

// TODO: Sort by alpha

#define kdlsym_addr_sys_lseek				0x0033D9F0
#define kdlsym_addr_sys_mmap				0x0013D230
#define kdlsym_addr_sys_munmap				0x0013D9A0
#define kdlsym_addr_utilUSleep				0x00658C30
#define kdlsym_addr_kthread_exit			0x138640
#define kdlsym_addr_kthread_add				0x00138360
#define kdlsym_addr_sys_read				0x00152AB0
#define kdlsym_addr_sys_fstat				0x000C1430
#define kdlsym_addr_sys_close				0x000C0EB0
#define kdlsym_addr_sys_socket				0x00318EE0
#define kdlsym_addr_sys_write				0x00152FC0


#define kdlsym_addr_sys_getdents			0x00341390
#define kdlsym_addr_sys_bind				0x00319820
#define kdlsym_addr_sys_listen				0x00319A60
#define kdlsym_addr_sys_accept				0x0031A170
#define kdlsym_addr_sys_recvfrom			0x0031B460
#define kdlsym_addr_sys_sendto				0x0031AD10
#define kdlsym_addr_sys_open				0x0033B990
#define kdlsym_addr_memcpy					0x1ea530
#define kdlsym_addr_memset					0x003205C0
#define kdlsym_addr_sys_stat				0x0033DFE0

#define	kdlsym_addr_Xfast_syscall           0x1c0
#define kdlsym_addr_sys_dup2				0x000BF050
#define kdlsym_addr_sys_shutdown			0x0031B6A0
#define kdlsym_addr_sys_unlink				0x0033D3D0
#define kdlsym_addr_sys_setuid				0x00054950
#define kdlsym_addr_sys_ptrace				0x0030D5E0
#define kdlsym_addr_sscanf					0x00175900
#define kdlsym_addr_mtx_init				0x00402780
#define kdlsym_addr_sys_mlock				0x0013E250
#define kdlsym_addr_sys_kill				0x000D19D0
#define kdlsym_addr_sys_mkdir				0x00340B50
#define kdlsym_addr_sys_rmdir				0x00340ED0

#define kdlsym_addr_eventhandler_register	0x001EC400
#define kdlsym_addr_prison0					0x10986A0
#define kdlsym_addr_rootvnode				0x22C1A70
#define kdlsym_addr_self_orbis_sysvec		0x019bbcd0
#define kdlsym_addr_trap_fatal				0x00171580
#endif

