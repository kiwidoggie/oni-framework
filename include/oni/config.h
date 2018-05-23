#pragma once

// 64-bit ARM FreeBSD based device
#define ONI_PLATFORM_AARCH64_BSD	1

// 64-bit x64 FreeBSD orbis based device
#define ONI_PLATFORM_ORBIS_BSD		2

// 64-bit x64 FreeBSD 9 based device
#define ONI_PLATFORM_9_BSD			3

// 32-bit ARM FreeBSD based device
#define ONI_PLATFORM_SAFE_BSD		4

// 4.55 Orbis Firmware
#define ONI_PLATFORM_ORBIS_BSD_455	5

// 5.01 Orbis Firmware
#define ONI_PLATFORM_ORBIS_BSD_501	6

// 5.05 Orbis Firmware
#define ONI_PLATFORM_ORBIS_BSD_505	7

// Unknown device
#define ONI_UNKNOWN_PLATFORM		-1

// Function address for kernel_execution
#ifndef ONI_KERN_EXEC
#define ONI_KERN_EXEC 0x4141414141414141
#endif

// The current platform configured by oni
#ifndef ONI_PLATFORM
#define ONI_PLATFORM ONI_PLATFORM_ORBIS_BSD_501
#endif

// The maximum number of plugins for use with oni
#define PLUGINMANAGER_MAX_PLUGINS	256

#ifdef _DEBUG
#define ONI_THREAD_NAME	"oni_thread"
#define ONI_LOG_BUFFER_SIZE	0x1000
#define ONI_BASE_PATH "/user/oni"
#define RPC_SLEEP "rpc sleep"
#define DEFAULT_AGENT_PORT 9999
#else
#define ONI_THREAD_NAME	""
#define ONI_LOG_BUFFER_SIZE	0x100
#define ONI_BASE_PATH "/user/config"
#define RPC_SLEEP ""
#define DEFAULT_AGENT_PORT	0
#endif