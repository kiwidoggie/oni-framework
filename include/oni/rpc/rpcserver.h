#pragma once

#include <sys/types.h>
#include <netinet/in.h>

#include <sys/param.h>
#include <sys/lock.h>
#include <sys/mutex.h>


#define RPCSERVER_MAX_CLIENTS	16
#define RPCSERVER_DEFAULT_PORT	9999

struct rpcconnection_t;
struct proc;
struct thread;

struct rpcserver_t
{
	struct rpcconnection_t* connections[RPCSERVER_MAX_CLIENTS];

	// Thread
	void* thread;
	int isRunning;

	// The server address
	struct sockaddr_in address;
	int socket;
	struct proc* process;

	struct mtx lock;
};

void rpcserver_init(struct rpcserver_t* server, struct proc* process);

int32_t rpcserver_findFreeClientIndex(struct rpcserver_t* server);
int32_t rpcserver_findClientIndex(struct rpcserver_t* server, struct rpcconnection_t* clientConnection);

uint32_t rpcserver_freeClientCount(struct rpcserver_t* server);

int32_t rpcserver_startup(struct rpcserver_t* server, uint16_t port);

uint8_t rpcserver_shutdown(struct rpcserver_t* server);

void rpcserver_serverThread(void* data);

void rpcserver_onClientDisconnect(struct rpcserver_t* server, struct rpcconnection_t* clientConnection);
