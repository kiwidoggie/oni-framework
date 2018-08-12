



#pragma once
#ifndef _WIN32
#include <sys/types.h>
#include <netinet/in.h>
#else
#include <inttypes.h>
#include <WinSock2.h>
#endif

#define RPCCONNECTION_BUFFER_SIZE	0x4000

struct rpcserver_t;
struct rpcconnection_t;

typedef void(*onClientDisconnect_t)(struct rpcserver_t*, struct rpcconnection_t*);

struct rpcconnection_t
{
	// Our tcp network socket
	int socket;

	// Should we terminate the thread
	int isRunning;

	// Thread that the socket operations are running on
	void* thread;

	// Buffer
	uint8_t buffer[RPCCONNECTION_BUFFER_SIZE];

	// Address of the client
	struct sockaddr_in address;

	// onClientDisconnect callback, set by the rpcserver
	onClientDisconnect_t disconnect;

	// Server reference
	void* server;

};

void rpcconnection_init(struct rpcconnection_t* connection);

void rpcconnection_clientThread(void* data);