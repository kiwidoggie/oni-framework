#pragma once
#include <oni/config.h>

#if ONI_PLATFORM==ONI_PLATFORM_ORBIS_BSD_501
/*
These are the required functions in order for the Oni Framework to operate properly
These are all offsets into the base of the kernel. They expect all standard FreeBSD 9 prototypes

The reason we do not hardcode offsets here, is due to the different platforms that are supported, and
for the platforms that do enable kernel ASLR (Address Space Layout Randomization?)
*/

#define kdlsym_addr_allproc						 		 	 0x02382FF8
#define kdlsym_addr_allproc_lock				 		 	 0x02382F98

#define kdlsym_addr_critical_enter						 	 0x0028E4D0
#define kdlsym_addr_critical_exit						 	 0x0028E4E0

#define	kdlsym_addr_kernel_map                               0x01AC60E0
#define kdlsym_addr_kern_reboot							 	 0x0010D280
#define kdlsym_addr_kmem_alloc 								 0x000fcb70
#define kdlsym_addr_kmem_free						 		 0x000FCD40
#define kdlsym_addr_kproc_create							 0x00137CE0

#define kdlsym_addr_sys_mlockall							 0x0013E1F0
#define kdlsym_addr_sys_mlock								 0x0013E140

#define kdlsym_addr_pfind						 		 	 0x00403110
#define kdlsym_addr_printf                                   0x00435c70
#define kdlsym_addr_proc_rwmem					 		 	 0x0030CDC0
#define kdlsym_addr_pmap_activate							 0x002EAC40

#define kdlsym_addr_vmspace_alloc							 0x0019EA10
#define kdlsym_addr_vmspace_free				 		 	 0x0019ECB0 

#define kdlsym_addr_snprintf					 		 	 0x00435F80

#define kdlsym_addr_vsnprintf					 		 	 0x00436020

#define kdlsym_addr__mtx_lock_flags				 		 	 0x00401900
#define kdlsym_addr__mtx_unlock_flags			 		 	 0x00401BD0

#define kdlsym_addr__sx_slock					 		 	 0x000F5B20
#define kdlsym_addr__sx_sunlock					 		 	 0x000F5E00

#define kdlsym_addr_vmspace_acquire_ref			 		 	 0x0019EE80
#define kdlsym_addr__vm_map_lock_read			 		 	 0x0019F030
#define kdlsym_addr__vm_map_unlock_read			 		 	 0x0019F080

// TODO: Sort by alpha

#define kdlsym_addr_sys_lseek					 		 	 0x0033D620
#define kdlsym_addr_sys_mmap					 		 	 0x0013D120
#define kdlsym_addr_sys_munmap					 		 	 0x0013D890
#define kdlsym_addr_utilUSleep							 	 0x00658850
#define kdlsym_addr_kthread_exit							 0x00138530
#define kdlsym_addr_kthread_add								 0x00138250
#define kdlsym_addr_sys_read                                 0x001529a0
#define kdlsym_addr_sys_fstat						 		 0x000C14B0
#define kdlsym_addr_sys_close                                0x000c0f30
#define kdlsym_addr_sys_socket                               0x00318b10
#define kdlsym_addr_sys_write                                0x00152eb0


#define kdlsym_addr_sys_getdents						 	 0x00340FC0
#define kdlsym_addr_sys_bind                                 0x00319450
#define kdlsym_addr_sys_listen                               0x00319690
#define kdlsym_addr_sys_accept                               0x00319da0
#define kdlsym_addr_sys_recvfrom							 0x0031B090
#define kdlsym_addr_sys_sendto								 0x0031A940
#define kdlsym_addr_sys_open							 	 0x0033B5C0
#define kdlsym_addr_memcpy                                   0x001ea420
#define kdlsym_addr_memset 								     0x3201F0
#define kdlsym_addr_sys_stat						 		 0x0033DC10

#define	kdlsym_addr_Xfast_syscall                            0x000001C0
#define kdlsym_addr_sys_dup2								 0x000BF0D0
#define kdlsym_addr_sys_shutdown							 0x0031B2D0
#define kdlsym_addr_sys_unlink								 0x0033D000
#define kdlsym_addr_sys_setuid								 0x00054950
#define kdlsym_addr_sys_ptrace								 0x0030D250
#define kdlsym_addr_sscanf									 0x001757F0
#define kdlsym_addr_mtx_init								 0x004023B0

#define kdlsym_addr_sys_kill								 0x000D1A50

#define kdlsym_addr_sys_mkdir								 0x00340780
#define kdlsym_addr_sys_rmdir								 0x00340B00
#endif
