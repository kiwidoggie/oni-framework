



#pragma once
#include <oni/utils/types.h>

#define RPCMESSAGE_HEADER_MAGIC 0xCC

struct allocation_t;

enum MSG_CATEGORY
{
	RPCCAT_NONE = 0,
	RPCCAT_SYSTEM,
	RPCCAT_LOG,
	RPCCAT_DBG,
	RPCCAT_FILE,
	RPCCAT_CMD,
	RPCCAT_COUNT
};

struct message_header_t
{
	// Magic (default: 0xCC)
	uint8_t magic;

	// If this message is a request or reply
	uint8_t request;

	// Payload length
	uint16_t payloadLength;

	// Category
	enum MSG_CATEGORY category;

	// User provided type
	uint32_t type;

	// Error code
	int32_t error;
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