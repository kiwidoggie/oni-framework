#pragma once
#include <oni/utils/types.h>

#include <sys/param.h>
#include <sys/proc.h>

struct task_struct;

/*
	kernelRdmsr

	TODO: Description
*/
uint64_t kernelRdmsr(int Register);
int proc_rw_mem(struct proc* p, void* ptr, size_t size, void* data, size_t* n, int write);