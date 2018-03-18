#pragma once
#include <oni/utils/types.h>

/*
	initparams_t

	Initialization Parameters
*/
struct initparams_t
{
	// Payload base address, can be allocated dynamically
	uint64_t payloadBase;

	// Payload size
	uint64_t payloadSize;

	// Kernel process handle
	struct proc* process;
};
