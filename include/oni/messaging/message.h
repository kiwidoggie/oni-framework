#pragma once
#include <oni/utils/types.h>

#define RPCMESSAGE_HEADER_MAGIC 0x05

struct allocation_t;

enum MSG_CATEGORY
{
	// Message has no category, this is considered invalid
	RPCCAT_NONE = 0,

	// Mira's system messages
	RPCCAT_SYSTEM,

	// Logging messages
	RPCCAT_LOG,

	// Debugger messages
	RPCCAT_DBG,

	// File messages
	RPCCAT_FILE,

	// Command messages
	RPCCAT_CMD,

	// Maximum amount of categories that are allowed, otherwise message struct needs changing
	RPCCAT_MAX = 14,
};

struct message_header_t
{
	// Header magic, default is 0101b or 5
	uint64_t magic : 3;

	// Message category, max 14
	uint64_t category : 4;

	// On requests this is the message type, on replies it is error code
	uint64_t error_type : 32;

	// Size of the payload that is going to be sent
	uint64_t payloadSize : 16;

	// Is this a request or response message
	uint64_t request : 1;

	// Reserved
	uint64_t reserved : 7;
};

//void message_init(struct message_header_t* message, int32_t socket);
//struct allocation_t* message_initParse(struct message_header_t* header, int32_t socket);

struct thread;
struct rpcserver_t;

int32_t rpcserver_findSocketFromThread(struct rpcserver_t* server, struct thread* td);