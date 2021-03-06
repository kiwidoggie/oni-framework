#pragma once
#include <oni/config.h>

#if ONI_PLATFORM==ONI_PLATFORM_ORBIS_BSD_501
/*
These are the required functions in order for the Oni Framework to operate properly
These are all offsets into the base of the kernel. They expect all standard FreeBSD 9 prototypes

The reason we do not hardcode offsets here, is due to the different platforms that are supported, and
for the platforms that do enable kernel ASLR (Address Space Layout Randomization?)
*/

#define kdlsym_addr__mtx_lock_flags                        0x00401900
#define kdlsym_addr__mtx_lock_sleep                        0x004019A0
#define kdlsym_addr__mtx_unlock_flags                      0x00401BD0
#define kdlsym_addr__mtx_unlock_sleep                      0x00401CD0
#define kdlsym_addr__sceSblAuthMgrGetSelfInfo              0x0063c960
#define kdlsym_addr__sx_slock                              0x000F5B20
#define kdlsym_addr__sx_sunlock                            0x000F5E00
#define kdlsym_addr__sx_xlock                              0x000F5D00
#define kdlsym_addr__sx_xunlock                            0x000F5EC0
#define kdlsym_addr__vm_map_lock_read                      0x0019F030
#define kdlsym_addr__vm_map_unlock_read                    0x0019F080
#define kdlsym_addr_AesCbcCfb128Decrypt                    0x003A2A30
#define kdlsym_addr_AesCbcCfb128Encrypt                    0x003A2800
#define kdlsym_addr_allproc                                0x02382FF8
#define kdlsym_addr_allproc_lock                           0x02382F98
#define kdlsym_addr_copyin                                 0x001EA600
#define kdlsym_addr_copyinstr                              0x001EAA30
#define kdlsym_addr_critical_enter                         0x0028E4D0
#define kdlsym_addr_critical_exit                          0x0028E4E0
#define kdlsym_addr_dmem_start_app_process                 0x002468E0
#define kdlsym_addr_eventhandler_register                  0x001EC2F0
#define kdlsym_addr_exec_new_vmspace                       0x0038A940
#define kdlsym_addr_faultin                                0x00006DD0
#define kdlsym_addr_fget_unlocked                          0x000C3530
#define kdlsym_addr_fpu_ctx                                0x0274C040
#define kdlsym_addr_fpu_kern_enter                         0x001BFE80
#define kdlsym_addr_fpu_kern_leave                         0x001BFF80
#define kdlsym_addr_free                                   0x0010E350
#define kdlsym_addr_gpu_va_page_list                       0x0271E208
#define kdlsym_addr_icc_nvs_read                           0x00395460
#define kdlsym_addr_kern_close                             0x000C0F40
#define kdlsym_addr_kern_mkdirat                           0x00340800
#define kdlsym_addr_kern_open                              0x0072AB50
#define kdlsym_addr_kern_openat                            0x0033B640
#define kdlsym_addr_kern_readv                             0x00152A10
#define kdlsym_addr_kern_reboot                            0x0010D280
#define kdlsym_addr_kern_sysents                           0x0107C610
#define kdlsym_addr_kernel_map                             0x01AC60E0
#define kdlsym_addr_kmem_alloc                             0x000FCB70
#define kdlsym_addr_kmem_free                              0x000FCD40
#define kdlsym_addr_kproc_create                           0x00137CE0
#define kdlsym_addr_kthread_add                            0x00138250
#define kdlsym_addr_kthread_exit                           0x00138530
#define kdlsym_addr_M_MOUNT                                0x019BF300
#define kdlsym_addr_M_TEMP                                 0x014B4110
#define kdlsym_addr_malloc                                 0x0010E140
#define kdlsym_addr_memcmp                                 0x00050AC0
#define kdlsym_addr_memcpy                                 0x001EA420
#define kdlsym_addr_memset                                 0x003201F0
#define kdlsym_addr_mini_syscore_self_binary               0x014C9D48
#define kdlsym_addr_mtx_init                               0x004023B0
#define kdlsym_addr_mtx_lock_sleep                         0x004019a0
#define kdlsym_addr_mtx_unlock_sleep                       0x00401bd0
#define kdlsym_addr_pfind                                  0x00403110
#define kdlsym_addr_pmap_activate                          0x002EAC40
#define kdlsym_addr_printf                                 0x00435C70
#define kdlsym_addr_prison0                                0x010986A0
#define kdlsym_addr_proc0                                  0x01AA4600
#define kdlsym_addr_proc_reparent                          0x00035330
#define kdlsym_addr_proc_rwmem                             0x0030CDC0
#define kdlsym_addr_realloc                                0x0010E480
#define kdlsym_addr_rootvnode                              0x022C19F0
#define kdlsym_addr_RsaesPkcs1v15Dec2048CRT                0x001FD6C0
#define kdlsym_addr_sbl_eap_internal_partition_key         0x02790C90
#define kdlsym_addr_sbl_pfs_sx                             0x0271E5D8
#define kdlsym_addr_sceSblAuthMgrIsLoadable2               0x0063C110
#define kdlsym_addr_sceSblAuthMgrSmStart                   0x00641500
#define kdlsym_addr_sceSblAuthMgrSmVerifyHeader            0x00642760
#define kdlsym_addr_sceSblAuthMgrVerifyHeader              0x0063C170
#define kdlsym_addr_sceSblDriverSendMsg                    0x0061D410
#define kdlsym_addr_sceSblGetEAPInternalPartitionKey       0x00625300
#define kdlsym_addr_sceSblKeymgrClearKey                   0x0062D730
#define kdlsym_addr_sceSblKeymgrSetKeyForPfs               0x0062D3A0
#define kdlsym_addr_sceSblKeymgrSmCallfunc                 0x0062DEC0
#define kdlsym_addr_sceSblPfsSetKeys                       0x0061EBC0
#define kdlsym_addr_sceSblServiceMailbox                   0x00632160
#define kdlsym_addr_self_orbis_sysvec                      0x019BBCD0
#define kdlsym_addr_Sha256Hmac                             0x002D52E0
#define kdlsym_addr_snprintf                               0x00435F80
#define kdlsym_addr_sscanf                                 0x001757F0
#define kdlsym_addr_strcmp                                 0x001D0EC0
#define kdlsym_addr_strdup                                 0x001C1B20
#define kdlsym_addr_strlen                                 0x003B6DD0
#define kdlsym_addr_strstr                                 0x0017DEA0
#define kdlsym_addr_sys_accept                             0x00319DA0
#define kdlsym_addr_sys_bind                               0x00319450
#define kdlsym_addr_sys_close                              0x000C0F30
#define kdlsym_addr_sys_dup2                               0x000BF0D0
#define kdlsym_addr_sys_fstat                              0x000C14B0
#define kdlsym_addr_sys_getdents                           0x00340FC0
#define kdlsym_addr_sys_kill                               0x000D1A50
#define kdlsym_addr_sys_listen                             0x00319690
#define kdlsym_addr_sys_lseek                              0x0033D620
#define kdlsym_addr_sys_mkdir                              0x00340780
#define kdlsym_addr_sys_mlock                              0x0013E140
#define kdlsym_addr_sys_mlockall                           0x0013E1F0
#define kdlsym_addr_sys_mmap                               0x0013D120
#define kdlsym_addr_sys_munmap                             0x0013D890
#define kdlsym_addr_sys_nmount                             0x001DE1D0
#define kdlsym_addr_sys_open                               0x0033B5C0
#define kdlsym_addr_sys_ptrace                             0x0030D250
#define kdlsym_addr_sys_read                               0x001529A0
#define kdlsym_addr_sys_recvfrom                           0x0031B090
#define kdlsym_addr_sys_rmdir                              0x00340B00
#define kdlsym_addr_sys_sendto                             0x0031A940
#define kdlsym_addr_sys_setuid                             0x00054950
#define kdlsym_addr_sys_shutdown                           0x0031B2D0
#define kdlsym_addr_sys_socket                             0x00318B10
#define kdlsym_addr_sys_stat                               0x0033DC10
#define kdlsym_addr_sys_unlink                             0x0033D000
#define kdlsym_addr_sys_unmount                            0x001DFB60
#define kdlsym_addr_sys_wait4                              0x00035470
#define kdlsym_addr_sys_write                              0x00152EB0
#define kdlsym_addr_trap_fatal                             0x00171470
#define kdlsym_addr_utilUSleep                             0x00658850
#define kdlsym_addr_vm_map_lookup_entry                    0x0019F650
#define kdlsym_addr_vmspace_acquire_ref                    0x0019EE80
#define kdlsym_addr_vmspace_alloc                          0x0019EA10
#define kdlsym_addr_vmspace_free                           0x0019ECB0
#define kdlsym_addr_vsnprintf                              0x00436020
#define kdlsym_addr_Xfast_syscall                          0x000001C0
#define kdlsym_addr_wakeup                                 0x003FB570

#endif