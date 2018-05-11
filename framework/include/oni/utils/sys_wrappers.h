#pragma once
#include <oni/utils/types.h>

struct task_struct;
struct proc;
struct sockaddr;
struct cdev;
struct cdevsw;
struct ucred;
struct thread;
struct stat;

extern int mlock(void* address, uint64_t size);
extern int mlockall(int how);
extern uint16_t khtons(uint16_t);
extern void kclose(int socket);
extern int ksocket(int, int, int);
extern int kbind(int, const struct sockaddr*, size_t);
extern int klisten(int, int);
extern int kaccept(int, struct sockaddr*, size_t*);
extern int krecv(int, void*, int, int);
extern int kopen(char* path, int flags, int mode);
extern ssize_t kwrite(int d, const void* buf, size_t nbytes);
extern int kgetdents(int fd, char* buf, int nbytes);
extern ssize_t kread(int fd, void* buf, size_t count);
extern int kfstat(int fd, struct stat* sb);
extern int kstat(char* path, struct stat* buf);
extern int kunlink(char* path);
extern int ksetuid(uid_t uid);
extern int kptrace(int req, pid_t pid, caddr_t addr, int data);
extern int kkill(int pid, int signum);

extern int kdup2(int oldd, int newd);
extern int kshutdown(int s, int how);

extern off_t klseek(int fd, off_t offset, int whence);
extern caddr_t kmmap(caddr_t addr, size_t len, int prot, int flags, int fd, off_t pos);
extern int kmunmap(void *addr, size_t len);

extern int kproc_create(void(*func)(void *), void *arg, struct proc **newpp, int flags, int	pages, const char *fmt, ...);
extern int kthread_add(void(*func)(void*), void* arg, struct proc* procp, struct thread** newtdpp, int flags, int pages, const char* fmt, ...);
extern int kthread_stop(struct task_struct *task);
extern void kthread_exit(void);

extern void *kmemset(void *s, int c, size_t n);
extern void *kmemcpy(void *dest, const void *src, size_t n);
extern void *kcalloc(size_t n, size_t size);

extern void utilUSleep(int nanoseconds, const char* excuse);

int proc_rw_mem(struct proc* p, void* ptr, size_t size, void* data, size_t* n, int write);