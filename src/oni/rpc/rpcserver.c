#include <oni/rpc/rpcserver.h>
#include <oni/rpc/rpcconnection.h>

#include <oni/config.h>

#include <oni/utils/memory/allocator.h>
#include <oni/utils/logger.h>
#include <oni/utils/sys_wrappers.h>

#include <sys/socket.h>

void rpcserver_init(struct rpcserver_t* server, struct proc* process)
{
	WriteLog(LL_Debug, "rpcserver initialization");

	if (!server)
	{
		WriteLog(LL_Error, "bad server object");
		return;
	}

	if (!process)
	{
		WriteLog(LL_Error, "bad process");
		return;
	}

	// Initalize all clients
	WriteLog(LL_Debug, "zeroing connections");
	for (uint32_t i = 0; i < ARRAYSIZE(server->connections); ++i)
		server->connections[i] = NULL;

	server->thread = NULL;
	server->isRunning = false;

	// Zero out the address buffer
	server->address.sin_family = 0;
	server->address.sin_port = 0;
	server->address.sin_family = 0;

	server->socket = -1;

	server->process = process;
}

int32_t rpcserver_findFreeClientIndex(struct rpcserver_t* server)
{
	// Verify server object
	if (!server)
		return -1;

	// Iterate through all of the server clients
	for (uint32_t i = 0; i < ARRAYSIZE(server->connections); ++i)
	{
		if (!server->connections[i])
			return i;
	}

	return -1;
}

uint32_t rpcserver_freeClientCount(struct rpcserver_t* server)
{
	// Verify that our server object is good
	if (!server)
		return 0;

	// Hold the current client count
	uint32_t clientCount = 0;

	// Calculcate the current client count
	for (uint32_t i = 0; i < ARRAYSIZE(server->connections); ++i)
	{
		if (server->connections[i])
			clientCount++;
	}

	// Return the client count
	return clientCount;
}

int32_t rpcserver_startup(struct rpcserver_t* server, uint16_t port)
{
	WriteLog(LL_Debug, "entered rpcserver_startup.");
	// Verify that our server object is valid
	if (!server)
	{
		WriteLog(LL_Error, "invalid server.");
		return 0;
	}

	// If the default port was specified, start on the default port
	if (port == 0)
		port = RPCSERVER_DEFAULT_PORT;

	// Create a new socket
	server->socket = ksocket(AF_INET, SOCK_STREAM, 0);
	if (server->socket == -1)
	{
		WriteLog(LL_Error, "could not create socket.");
		return 0;
	}

	WriteLog(LL_Debug, "Socket Created: %d", server->socket);

	// Set up the server address structure
	kmemset(&server->address, 0, sizeof(server->address));

	server->address.sin_family = AF_INET;
	server->address.sin_addr.s_addr = INADDR_ANY;
	server->address.sin_port = __bswap16(port);
	server->address.sin_len = sizeof(server->address);

	// Bind the server to the provided port
	int ret = kbind(server->socket, (struct sockaddr*)&server->address, sizeof(server->address));
	if (ret < 0)
	{
		kshutdown(server->socket, 2);
		kclose(server->socket);
		server->socket = -1;

		WriteLog(LL_Error, "could not bind the socket %d.", ret);
		return 0;
	}

	WriteLog(LL_Debug, "socket bound.");

	// Listen for clients
	if (klisten(server->socket, 3) == -1)
	{
		kshutdown(server->socket, 2);
		kclose(server->socket);
		server->socket = -1;

		WriteLog(LL_Error, "could not listen to the socket.");
		return 0;
	}

	int creationResult = kthread_add(rpcserver_serverThread, server, server->process, (struct thread**)&server->thread, 0, 0, "oni_rpcserver");
	if (creationResult != 0)
		return 0;

	WriteLog(LL_Debug, "rpcServer thread started.");

	return 1;
}

void rpcserver_serverThread(void* data)
{
	WriteLog(LL_Debug, "entered serverThread.");

	if (!data)
	{
		WriteLog(LL_Error, "invalid data.");
		kthread_exit();
		return;
	}

	struct rpcserver_t* server = (struct rpcserver_t*)data;

	// Set the running flag
	server->isRunning = TRUE;

	WriteLog(LL_Debug, "server running.");

	// Create a new client connection
	struct rpcconnection_t* clientConnection = (struct rpcconnection_t*)kmalloc(sizeof(struct rpcconnection_t));
	if (!clientConnection)
	{
		WriteLog(LL_Error, "could not allocate rpcconnection_t object.");
		server->isRunning = FALSE;
		kthread_exit();
		return;
	}

	// Initialize our new client connection
	rpcconnection_init(clientConnection);
	rpcconnection_initializeBuffers(clientConnection);

	// Assign callbacks
	clientConnection->server = server;
	clientConnection->disconnect = rpcserver_onClientDisconnect;

	WriteLog(LL_Debug, "waiting for clients...");

	size_t clientAddressSize = sizeof(clientConnection->address);
	while ((clientConnection->socket = kaccept(server->socket, (struct sockaddr*)&clientConnection->address, &clientAddressSize)) > 0)
	{
		// Check to see if we should continue running
		if (!server->isRunning)
			break;

		WriteLog(LL_Debug, "accepted first client.");

		// If we got an invalid socket, then just try again. I'm not sure what is supposed to happen in this case
		if (clientConnection->socket == -1)
			continue;

		int32_t clientIndex = rpcserver_findFreeClientIndex(server);
		if (clientIndex == -1)
		{
			// Close the socket and re-use what we already got
			kshutdown(clientConnection->socket, 2);
			kclose(clientConnection->socket);
			clientConnection->socket = -1;
			continue;
		}

		WriteLog(LL_Debug, "found free rpc client.");

		// Create the new client thread which will handle dispatching

		int creationResult = kthread_add(rpcconnection_serverThread, clientConnection, server->process, (struct thread**)&clientConnection->thread, 0, 0, "oni_client");
		if (creationResult != 0)
		{
			WriteLog(LL_Error, "could not create client thread.");
			continue;
		}

		// Assign our server the connection for later
		server->connections[clientIndex] = clientConnection;
		WriteLog(LL_Debug, "client added.");

		// Intentionally do not free the previous object, since now we have a reference to it in the server and allocate a new object
		clientConnection = (struct rpcconnection_t*)kmalloc(sizeof(struct rpcconnection_t));
		if (!clientConnection)
		{
			WriteLog(LL_Error, "could not allocate another rpcconnection_t object.");
			break;
		}

		// Initialize our connection
		rpcconnection_init(clientConnection);
		rpcconnection_initializeBuffers(clientConnection);

		// Assign callbacks
		clientConnection->server = server;
		clientConnection->disconnect = rpcserver_onClientDisconnect;
	}

	WriteLog(LL_Debug, "shutting down.");

	kthread_exit();
}

int32_t rpcserver_shutdown(struct rpcserver_t* server)
{
	if (!server)
		return false;

	server->isRunning = false;

	// Stop all connections
	for (uint32_t i = 0; i < ARRAYSIZE(server->connections); ++i)
		rpcconnection_shutdown(server->connections[i]);

	// Free all of the server bullshit
	if (server->socket != -1)
	{
		kshutdown(server->socket, 2);
		kclose(server->socket);
		server->socket = -1;
	}

	// This stops the thread and waits for exit
	if (server->thread)
	{
		kthread_stop(server->thread);
		server->thread = NULL;
	}

	// Zero address space
	kmemset(&server->address, 0, sizeof(server->address));

	return true;
}

int32_t rpcserver_findClientIndex(struct rpcserver_t* server, struct rpcconnection_t* clientConnection)
{
	// Verify our arguments
	if (!server || !clientConnection)
		return -1;

	// Iterate through all of the connections and see if they match
	for (int32_t i = 0; i < ARRAYSIZE(server->connections); ++i)
	{
		if (server->connections[i] == clientConnection)
			return i;
	}

	// Connection not found
	return -1;
}

void rpcserver_onClientDisconnect(struct rpcserver_t* server, struct rpcconnection_t* clientConnection)
{
	if (!server || !clientConnection)
		return;

	// Remove the index
	int32_t clientIndex = rpcserver_findClientIndex(server, clientConnection);
	if (clientIndex == -1)
		return;

	// Remove it from our list
	server->connections[clientIndex] = NULL;

	// Free the connection
	kfree(clientConnection, sizeof(*clientConnection));

	WriteLog(LL_Debug, "onClientDisconnect: %d", clientIndex);
}