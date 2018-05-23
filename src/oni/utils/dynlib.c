#include <oni/utils/dynlib.h>
#include <oni/utils/syscall.h>

int64_t sys_dynlib_load_prx(char* prxPath)
{
	return (int64_t)syscall1(594, prxPath);
}

int64_t sys_dynlib_unload_prx(int64_t prxID)
{
	return (int64_t)syscall1(595, (void*)prxID);
}

int64_t sys_dynlib_dlsym(int64_t moduleHandle, const char* functionName, void *destFuncOffset)
{
	return (int64_t)syscall3(591, (void*)moduleHandle, (void*)functionName, destFuncOffset);
}