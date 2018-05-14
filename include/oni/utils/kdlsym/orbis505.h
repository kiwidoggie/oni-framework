#pragma once

#if ONI_PLATFORM==ONI_PLATFORM_ORBIS_BSD_505
/*
These are the required functions in order for the Oni Framework to operate properly
These are all offsets into the base of the kernel. They expect all standard FreeBSD 9 prototypes

The reason we do not hardcode offsets here, is due to the different platforms that are supported, and
for the platforms that do enable kernel ASLR (Address Space Layout Randomization?)
*/
#define	kdlsym_addr_Xfast_syscall                              0x1c0
#define	kdlsym_addr_faultin                                    0x6dd0
#define	kdlsym_addr_sceSblACMgrIsAllowedSystemLevelDebugging   0x11730
#define	kdlsym_addr_sceSblACMgrIsAllowedCoredump               0x11750
#define	kdlsym_addr_sceSblACMgrHasMmapSelfCapability           0x117b0
#define	kdlsym_addr_sceSblACMgrIsAllowedToMmapSelf             0x117c0
#define	kdlsym_addr_proc_reparent                              0x35330
#define	kdlsym_addr_memcmp                                     0x50ac0
#define	kdlsym_addr_strcpy                                     0x8f250
#define	kdlsym_addr_kern_psignal                               0xd36d0
#define	kdlsym_addr__sx_slock                                  0xf5c30
#define	kdlsym_addr__sx_xlock                                  0xf5e10
#define	kdlsym_addr__sx_sunlock                                0xf5f10
#define	kdlsym_addr__sx_xunlock	                               0xf5fd0
#define	kdlsym_addr_kmem_alloc                                 0xfcc80
#define	kdlsym_addr_kmem_free                                  0xfce50
#define	kdlsym_addr_panic                                      0x10daf0
#define	kdlsym_addr_malloc                                     0x10e250
#define	kdlsym_addr_free                                       0x10e460
#define	kdlsym_addr_kproc_create                               0x137df0
#define	kdlsym_addr_kproc_exit                                 0x138060
#define	kdlsym_addr_kthread_add                                0x138360
#define	kdlsym_addr_kthread_exit                               0x138640
#define	kdlsym_addr_vm_mmap                                    0x13e590
#define kdlsym_addr_trap_fatal                                 0x171580
#define	kdlsym_addr_vmspace_alloc                              0x19eb20
#define	kdlsym_addr_vmspace_free                               0x19edc0
#define	kdlsym_addr_vmspace_acquire_ref                        0x19ef90
#define	kdlsym_addr__vm_map_lock                               0x19eff0
#define	kdlsym_addr__vm_map_unlock                             0x19f060
#define	kdlsym_addr__vm_map_lock_read                          0x19f140
#define	kdlsym_addr__vm_map_unlock_read                        0x19f190
#define	kdlsym_addr_vm_map_insert                              0x1a0280
#define	kdlsym_addr_vm_map_fixed                               0x1a2080
#define	kdlsym_addr_vm_map_stack                               0x1a8090
#define	kdlsym_addr_tdfind                                     0x1b3ae0
#define	kdlsym_addr_strncmp                                    0x1b8fe0
#define	kdlsym_addr_namei                                      0x1c9780
#define	kdlsym_addr_NDFREE                                     0x1cb170
#define	kdlsym_addr_strcmp                                     0x1d0fd0
#define	kdlsym_addr_vm_object_reference                        0x1e4a60
#define	kdlsym_addr_vm_object_deallocate                       0x1e4ae0
#define	kdlsym_addr_bzero                                      0x1ea470
#define	kdlsym_addr_memcpy                                     0x1ea530
#define kdlsym_addr_copyout                                    0x1ea630
#define kdlsym_addr_copyin                                     0x1ea710
#define	kdlsym_addr_index                                      0x218a00
#define	kdlsym_addr_spinlock_enter                             0x234a70
#define	kdlsym_addr_spinlock_exit                              0x234ab0
#define	kdlsym_addr_kdlsym_sysctl                              0x262290
#define	kdlsym_addr_vrele                                      0x27bb40
#define	kdlsym_addr_critical_enter                             0x28e7a0
#define	kdlsym_addr_critical_exit                              0x28e7b0
#define	kdlsym_addr_pmap_activate                              0x2eafd0
#define	kdlsym_addr_proc_rwmem                                 0x30d150
#define	kdlsym_addr_sched_sleep                                0x31ee90
#define	kdlsym_addr_memset                                     0x3205c0
#define	kdlsym_addr_aslr_gen_pseudo_random_number              0x3889b0
#define	kdlsym_addr_icc_nvs_write                              0x395670
#define	kdlsym_addr_icc_nvs_read                               0x395830
#define	kdlsym_addr_strlen                                     0x3b71a0
#define	kdlsym_addr_pause                                      0x3fb920
#define	kdlsym_addr_wakeup                                     0x3fb940
#define	kdlsym_addr_setrunnable                                0x3fbb40
#define	kdlsym_addr__mtx_lock_flags                            0x401cd0
#define	kdlsym_addr__mtx_lock_sleep                            0x401d70
#define	kdlsym_addr__mtx_unlock_flags                          0x401fa0
#define	kdlsym_addr__mtx_unlock_sleep                          0x4020a0
#define	kdlsym_addr__mtx_unlock_spin_flags                     0x4022c0
#define	kdlsym_addr__thread_lock_flags                         0x402420
#define	kdlsym_addr_mtx_init                                   0x402780
#define	kdlsym_addr_pfind                                      0x4034e0
#define	kdlsym_addr_uprintf                                    0x4358b0
#define	kdlsym_addr_printf                                     0x436040
#define	kdlsym_addr_snprintf                                   0x436350
#define	kdlsym_addr_rindex                                     0x4859a0
#define kdlsym_addr_sceRegMgrSetInt                            0x4f8d10
#define kdlsym_addr_sceRegMgrGetInt                            0x4f9e50
#define	kdlsym_addr_avcontrol_sleep                            0x6eafb0
#define	kdlsym_addr_sysdump_perform_dump_on_fatal_trap         0x7673e0
#define	kdlsym_addr_kdlsym_max_text_size                       0x76b894
#define	kdlsym_addr_prison0                                    0x10986a0
#define	kdlsym_addr_M_TEMP                                     0x14b4110
#define	kdlsym_addr_self_orbis_sysvec                          0x19bbcd0
#define	kdlsym_addr_proctree_lock                              0x19efbf0
#define	kdlsym_addr_maxtsiz                                    0x1ace910
#define	kdlsym_addr_maxssiz                                    0x1ace930
#define	kdlsym_addr_sgrowsiz                                   0x1ace938
#define	kdlsym_addr_kdlsym_map                                 0x1ac60e0
#define	kdlsym_addr_global_settings_base                       0x1cd0650
#define	kdlsym_addr_rootvnode                                  0x22c1a70
#define	kdlsym_addr_allproc_lock                               0x2382f98
#define	kdlsym_addr_allproc                                    0x2382ff8
#endif