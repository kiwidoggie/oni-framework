#include <oni/rpc/rpcconnection.h>
#include <oni/config.h>

#include <oni/messaging/message.h>
#include <oni/messaging/messagemanager.h>

#include <oni/utils/memory/allocator.h>
#include <oni/utils/logger.h>
#include <oni/utils/sys_wrappers.h>
#include <oni/utils/kdlsym.h>

#include <oni/framework.h>

void rpcconnection_init(struct rpcconnection_t* connection)
/*
Initializes a new rpcconnection_t object
*/
{
	void* (*memset)(void *s, int c, size_t n) = kdlsym(memset);

	// Verify that our connection object is fine
	if (!connection)
		return;

	// Set the socket to invalid socket value
	connection->socket = -1;

	// Zero out thread information
	connection->thread = 0;
	connection->isRunning = 0;

	// Zero out the buffer and the buffer size
	memset(connection->buffer, 0, sizeof(connection->buffer));

	// Zero out the address buffer
	connection->address.sin_family = 0;
	connection->address.sin_port = 0;
	connection->address.sin_family = 0;
	connection->address.sin_len = sizeof(connection->address);
}

int32_t rpcconnection_initializeBuffers(struct rpcconnection_t* connection)
/*
Initializes the buffers

Returns: 1 on success, 0 otherwise
*/
{
	void* (*memset)(void *s, int c, size_t n) = kdlsym(memset);

	// Verify that our connection object is good
	if (!connection)
		return 0;

	// Allocate buffer data
	memset(connection->buffer, 0, sizeof(connection->buffer));

	return 1;
}

void rpcconnection_shutdown(struct rpcconnection_t* connection)
{
	void* (*memset)(void *s, int c, size_t n) = kdlsym(memset);

	if (!connection)
		return;

	// Manually update the status, the thread should auto-terminate
	connection->isRunning = FALSE;

	// If the socket is not invalid
	if (connection->socket != -1)
	{
		// Free the socket
		kshutdown(connection->socket, 2);

		kclose(connection->socket);
		connection->socket = -1;
	}

	// Stop the thread if it hasn't been already
	if (connection->thread)
	{
		// This stops the thread and waits for exit
		//kthread_stop(connection->thread);

		// Clear the thread
		connection->thread = NULL;
	}

	// Zero the buffer
	memset(connection->buffer, 0, sizeof(connection->buffer));

	// Clear out the address information
	memset(&connection->address, 0, sizeof(connection->address));

	// Invoke the onClientDisconnect handler on the server side to remove from the list and free this connection
	if (connection->disconnect)
		connection->disconnect(connection->server, connection);
}

void rpcconnection_serverThread(void* data)
{
	void(*kthread_exit)(void) = kdlsym(kthread_exit);
	void* (*memset)(void *s, int c, size_t n) = kdlsym(memset);
	void* (*memcpy)(void* dest, const void* src, size_t n) = kdlsym(memcpy);

	WriteLog(LL_Debug, "rpcconnection_serverThread entered");

	// Verify that our data pointer is valid
	if (!data)
	{
		kthread_exit();
		return;
	}

	struct rpcconnection_t* connection = (struct rpcconnection_t*)data;
	// Initialize all of the buffers
	WriteLog(LL_Debug, "rpcconnection_serverThread init buffers");
	if (!rpcconnection_initializeBuffers(connection))
	{
		kthread_exit();
		return;
	}

	// Set the running status
	connection->isRunning = true;

	const uint32_t messageHeaderSize = sizeof(struct message_header_t);
	uint32_t dataReceived = 0;

	// While we have the server running
	while (connection->isRunning)
	{
		WriteLog(LL_Debug, "rpcconnection_serverThread zeroing buffer");
		// Zero out the buffer
		memset(connection->buffer, 0, sizeof(connection->buffer));

		// If the connection socket has an error state
		if (connection->socket < 0)
		{
			WriteLog(LL_Error, "socket error: %d", connection->socket);
			goto cleanup;
		}

		WriteLog(LL_Debug, "rpcconnection_serverThread begin recv");
		dataReceived = 0;

		// Try to get the size of a full message
		int32_t recvSize = krecv(connection->socket, (char*)connection->buffer, messageHeaderSize, 0);
		if (recvSize < 0)
		{
			WriteLog(LL_Error, "%d recv returned %d.", connection->socket, recvSize);
			goto cleanup;
		}

		// Increment the dataReceived
		dataReceived += recvSize;

		// Otherwise, try to get the entire full message
		while (dataReceived < messageHeaderSize)
		{
			WriteLog(LL_Debug, "rpcconnection_serverThread recv more %d", dataReceived);
			uint32_t wantRecvSize = messageHeaderSize - dataReceived;

			recvSize = krecv(connection->socket, &connection->buffer[dataReceived], wantRecvSize, 0);
			if (recvSize <= 0)
			{
				WriteLog(LL_Error, "recv err returned %d.", recvSize);
				goto cleanup;
			}

			dataReceived += recvSize;
		}

		WriteLog(LL_Debug, "checking message header");

		// Check the message
		struct message_header_t* header = (struct message_header_t*)connection->buffer;
		if (header->magic != RPCMESSAGE_HEADER_MAGIC)
		{
			WriteLog(LL_Error, "invalid header got 0x%02x expected 0xCC", header->magic);
			goto cleanup;
		}
			
		// Check to see how much data we actually got
		uint64_t totalDataSize = recvSize + header->payloadSize;

		WriteLog(LL_Debug, "checking payload length\n");
		// If the payload length is bigger than the maximum buffer size, then fail
		if (totalDataSize > ARRAYSIZE(connection->buffer))
		{
			WriteLog(LL_Error, "payload length greater than buffer size.");
			goto cleanup;
		}

		WriteLog(LL_Debug, "total data size %lld", totalDataSize);
		// Recv the payload
		while (dataReceived < totalDataSize)
		{
			uint64_t dataSizeRemaining = totalDataSize - dataReceived;
			recvSize = krecv(connection->socket, (char*)(connection->buffer) + dataReceived, dataSizeRemaining & 0xFFFFFFFF, 0);
			if (recvSize < 0)
				goto cleanup;

			dataReceived += recvSize;
		}

		WriteLog(LL_Debug, "creating internal message");
		// Create a new "local" message
		struct allocation_t* allocation = message_initParse(header, connection->socket);
		if (!allocation)
		{
			WriteLog(LL_Error, "could not allocate memory for message");
			continue;
		}

		struct message_t* internalMessage = __get(allocation);
		WriteLog(LL_Debug, "internal message %p", internalMessage);
		if (!internalMessage)
			continue;

		internalMessage->payload = NULL;

		// Allow us to send header-only messages
		if (header->payloadSize != NULL)
		{
			WriteLog(LL_Debug, "allocating payload length %d", header->payloadSize);
			internalMessage->payload = kmalloc(header->payloadSize);
			if (!internalMessage->payload)
			{
				WriteLog(LL_Error, "error allocating payload");
				break;
			}

			WriteLog(LL_Debug, "zeroing payload");
			memset(internalMessage->payload, 0, header->payloadSize);

			WriteLog(LL_Debug, "copying payload");
			memcpy(internalMessage->payload, (((char*)(connection->buffer)) + sizeof(struct message_header_t)), header->payloadSize);

		}
		
		WriteLog(LL_Debug, "dispatching message %p %p", gFramework->messageManager, allocation);

		// Now that we have the full message chunk, parse the category and the type and get it the fuck outa here
		messagemanager_sendMessage(gFramework->messageManager, allocation);

		__dec(allocation);
	}

	
cleanup:
	connection->isRunning = false;
	rpcconnection_shutdown(connection);
	kthread_exit();
}