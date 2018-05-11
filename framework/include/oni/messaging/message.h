



#pragma once
#include <oni/utils/types.h>

#define RPCMESSAGE_HEADER_MAGIC 0x05

struct allocation_t;

enum MSG_CATEGORY
{
	RPCCAT_NONE = 0,
	RPCCAT_SYSTEM,
	RPCCAT_LOG,
	RPCCAT_DBG,
	RPCCAT_FILE,
	RPCCAT_CMD,
	RPCCAT_MAX = 14,
	RPCCAT_COUNT
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

struct message_t
{
	// Initial header
	struct message_header_t header;

	// Socket descritpr
	int32_t socket;

	// Payload
	void* payload;
};

void message_init(struct message_t* message, int32_t socket);
struct allocation_t* message_initParse(struct message_header_t* header, int32_t socket);