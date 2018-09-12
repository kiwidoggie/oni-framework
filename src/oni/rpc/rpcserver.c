#include <oni/rpc/rpcserver.h>
#include <oni/rpc/rpcconnection.h>

#include <oni/config.h>

#include <oni/utils/memory/allocator.h>
#include <oni/utils/logger.h>
#include <oni/utils/sys_wrappers.h>
#include <oni/utils/kdlsym.h>
#include <oni/utils/escape.h>

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

	void(*mtx_init)(struct mtx *m, const char *name, const char *type, int opts) = kdlsym(mtx_init);
	mtx_init(&server->lock, "rpcmtx", NULL, 0);
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
	int(*kthread_add)(void(*func)(void*), void* arg, struct proc* procptr, struct thread** tdptr, int flags, int pages, const char* fmt, ...) = kdlsym(kthread_add);
	void* (*memset)(void *s, int c, size_t n) = kdlsym(memset);
	void(*_mtx_lock_flags)(struct mtx *m, int opts, const char *file, int line) = kdlsym(_mtx_lock_flags);
	void(*_mtx_unlock_flags)(struct mtx *m, int opts, const char *file, int line) = kdlsym(_mtx_unlock_flags);

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
	if (server->socket < 0)
	{
		WriteLog(LL_Error, "could not create socket.");
		return 0;
	}

	WriteLog(LL_Debug, "Socket Created: %d", server->socket);

	// Set up the server address structure
	memset(&server->address, 0, sizeof(server->address));

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
	if (klisten(server->socket, 3) < 0)
	{
		kshutdown(server->socket, 2);
		kclose(server->socket);
		server->socket = -1;

		WriteLog(LL_Error, "could not listen to the socket.");
		return 0;
	}

	uint8_t threadCreated = true;
	_mtx_lock_flags(&server->lock, 0, __FILE__, __LINE__);

	int creationResult = kthread_add(rpcserver_serverThread, server, server->process, (struct thread**)&server->thread, 0, 0, "oni_rpcserver");
	if (creationResult != 0)
		threadCreated = false;

	WriteLog(LL_Debug, "rpcServer thread %s.", threadCreated ? "successfully" : "unsuccessfully");

	_mtx_unlock_flags(&server->lock, 0, __FILE__, __LINE__);

	return threadCreated;
}

void rpcserver_serverThread(void* data)
{
	void(*kthread_exit)(void) = kdlsym(kthread_exit);
	int(*kthread_add)(void(*func)(void*), void* arg, struct proc* procptr, struct thread** tdptr, int flags, int pages, const char* fmt, ...) = kdlsym(kthread_add);
	void(*_mtx_lock_flags)(struct mtx *m, int opts, const char *file, int line) = kdlsym(_mtx_lock_flags);
	void(*_mtx_unlock_flags)(struct mtx *m, int opts, const char *file, int line) = kdlsym(_mtx_unlock_flags);

	if (!data)
	{
		WriteLog(LL_Error, "invalid data.");
		kthread_exit();
		return;
	}

	struct rpcserver_t* server = (struct rpcserver_t*)data;

	// This should block until it is freed in the previous context
	_mtx_lock_flags(&server->lock, 0, __FILE__, __LINE__);

	WriteLog(LL_Debug, "entered serverThread.");

	_mtx_unlock_flags(&server->lock, 0, __FILE__, __LINE__);

	oni_threadEscape(curthread, NULL);

	// Set the running flag
	server->isRunning = TRUE;

	WriteLog(LL_Debug, "server running.");

	struct timeval timeout;
	timeout.tv_sec = 5;
	timeout.tv_usec = 0;

	int32_t result = 0;/*ksetsockopt(server->socket, SOL_SOCKET, SO_RCVTIMEO, (caddr_t)&timeout, sizeof(timeout));
	if (result < 0)
	{
		WriteLog(LL_Error, "could not set recv timeout (%d).", result);
		goto cleanup;
	}

	result = ksetsockopt(server->socket, SOL_SOCKET, SO_SNDTIMEO, (caddr_t)&timeout, sizeof(timeout));
	if (result < 0)
	{
		WriteLog(LL_Error, "could not set send timeout (%d).", result);
		goto cleanup;
	}*/

	// SO_LINGER
	result = ksetsockopt(server->socket, SOL_SOCKET, SO_LINGER, (caddr_t)&timeout, sizeof(timeout));
	if (result < 0)
	{
		WriteLog(LL_Error, "could not set send timeout (%d).", result);
		goto cleanup;
	}

	// SO_LINGER
	timeout.tv_sec = 0;
	result = ksetsockopt(server->socket, SOL_SOCKET, SO_LINGER, (caddr_t)&timeout, sizeof(timeout));
	if (result < 0)
	{
		WriteLog(LL_Error, "could not set send timeout (%d).", result);
		goto cleanup;
	}

	// Create a new client connection
	struct rpcconnection_t* clientConnection = (struct rpcconnection_t*)kmalloc(sizeof(struct rpcconnection_t));
	if (!clientConnection)
	{
		WriteLog(LL_Error, "could not allocate rpcconnection_t object.");
		goto cleanup;
	}

	// Initialize our new client connection
	rpcconnection_init(clientConnection);

	// Assign callbacks
	clientConnection->server = server;
	clientConnection->disconnect = rpcserver_onClientDisconnect;

	WriteLog(LL_Debug, "waiting for clients...");

	size_t clientAddressSize = sizeof(clientConnection->address);

	while (server->isRunning)
	{
		clientConnection->socket = kaccept(server->socket, (struct sockaddr*)&clientConnection->address, &clientAddressSize);
		WriteLog(LL_Debug, "accepted first client (%d).", clientConnection->socket);

		// If we got an invalid socket, then just try again. I'm not sure what is supposed to happen in this case
		if (clientConnection->socket <= 0)
		{
			WriteLog(LL_Error, "could not accept client (%d)", clientConnection->socket);
			server->isRunning = false;
			goto cleanup;
		}

		//timeout.tv_sec = 0;
		//result = ksetsockopt(clientConnection->socket, SOL_SOCKET, SO_RCVTIMEO, (caddr_t)&timeout, sizeof(timeout));
		//if (result < 0)
		//{
		//	WriteLog(LL_Error, "could not set recv timeout (%d).", result);
		//	goto cleanup;
		//}

		//result = ksetsockopt(clientConnection->socket, SOL_SOCKET, SO_SNDTIMEO, (caddr_t)&timeout, sizeof(timeout));
		//if (result < 0)
		//{
		//	WriteLog(LL_Error, "could not set send timeout (%d).", result);
		//	goto cleanup;
		//}

		// Below is a stupid coding pattern to prevent multiple locks/unlocks

		int32_t clientIndex = rpcserver_findFreeClientIndex(server);
		if (clientIndex == -1)
		{
			// Close the socket and re-use what we already got
			kshutdown(clientConnection->socket, 2);
			kclose(clientConnection->socket);
			clientConnection->socket = -1;
			continue;
		}

		// Assign our server the connection for later
		WriteLog(LL_Debug, "client to be assigned to index %d.", clientIndex);
		server->connections[clientIndex] = clientConnection;
		WriteLog(LL_Debug, "client added.");

		// Create the new client thread which will handle dispatching
		int creationResult = kthread_add(rpcconnection_clientThread, clientConnection, server->process, (struct thread**)&clientConnection->thread, 0, 0, "oni_client");
		if (creationResult != 0)
		{
			WriteLog(LL_Error, "could not create client thread.");
			continue;
		}

		// Intentionally do not free the previous object, since now we have a reference to it in the server and allocate a new object
		clientConnection = (struct rpcconnection_t*)kmalloc(sizeof(struct rpcconnection_t));
		if (!clientConnection)
		{
			WriteLog(LL_Error, "could not allocate another rpcconnection_t object.");
			server->isRunning = false;
			goto cleanup;
		}

		// Initialize our connection
		rpcconnection_init(clientConnection);

		// Assign callbacks
		clientConnection->server = server;
		clientConnection->disconnect = rpcserver_onClientDisconnect;
	}
	
cleanup:
	WriteLog(LL_Debug, "rpc server has shutdown with result %d", rpcserver_shutdown(server));
	kthread_exit();
}

uint8_t rpcserver_shutdown(struct rpcserver_t* server)
{
	void(*_mtx_lock_flags)(struct mtx *m, int opts, const char *file, int line) = kdlsym(_mtx_lock_flags);
	void(*_mtx_unlock_flags)(struct mtx *m, int opts, const char *file, int line) = kdlsym(_mtx_unlock_flags);

	if (!server)
		return false;

	if (server->socket == -1)
		return false;

	server->isRunning = false;
	_mtx_lock_flags(&server->lock, 0, __FILE__, __LINE__);

	// Iterate through each of the connections and force connections to error and get cleaned up
	for (uint32_t i = 0; i < ARRAYSIZE(server->connections); ++i)
	{
		struct rpcconnection_t* connection = server->connections[i];
		if (!connection)
			continue;

		if (connection->socket < 0)
			continue;

		kshutdown(connection->socket, 2);
		kclose(connection->socket);
		connection->socket = -1;
	}

	// Shut down the actual server socket
	if (server->socket >= 0)
	{
		kshutdown(server->socket, 2);
		kclose(server->socket);
		server->socket = -1;
	}


	_mtx_unlock_flags(&server->lock, 0, __FILE__, __LINE__);

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
	void(*_mtx_lock_flags)(struct mtx *m, int opts, const char *file, int line) = kdlsym(_mtx_lock_flags);
	void(*_mtx_unlock_flags)(struct mtx *m, int opts, const char *file, int line) = kdlsym(_mtx_unlock_flags);

	if (!server || !clientConnection)
		return;

	_mtx_lock_flags(&server->lock, 0, __FILE__, __LINE__);

	// Remove the index
	int32_t clientIndex = rpcserver_findClientIndex(server, clientConnection);
	if (clientIndex != -1)
	{
		// Remove it from our list
		server->connections[clientIndex] = NULL;
	}

	// Free the connection
	kfree(clientConnection, sizeof(*clientConnection));

	WriteLog(LL_Debug, "onClientDisconnect: %d", clientIndex);
	_mtx_unlock_flags(&server->lock, 0, __FILE__, __LINE__);
}

int32_t rpcserver_findSocketFromThread(struct rpcserver_t * server, struct thread * td)
{
	if (!server || !td)
		return -1;

	// Iterate through all connections and compare threads
	for (uint32_t i = 0; i < ARRAYSIZE(server->connections); ++i)
	{
		struct rpcconnection_t* connection = server->connections[i];
		if (!connection)
			continue;

		if (connection->thread == td)
			return connection->socket;
	}

	return -1;
}