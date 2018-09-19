#include <oni/rpc/rpcconnection.h>
#include <oni/config.h>

#include <oni/messaging/message.h>
#include <oni/messaging/messagemanager.h>

#include <oni/utils/memory/allocator.h>
#include <oni/utils/logger.h>
#include <oni/utils/sys_wrappers.h>
#include <oni/utils/kdlsym.h>
#include <oni/utils/escape.h>
#include <oni/utils/ref.h>

#include <oni/framework.h>

#include <sys/malloc.h>

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

void rpcconnection_clientThread(void* data)
{
	void(*kthread_exit)(void) = kdlsym(kthread_exit);
	void* (*memset)(void *s, int c, size_t n) = kdlsym(memset);

	WriteLog(LL_Debug, "rpcconnection_serverThread entered");

	// Verify that our data pointer is valid
	if (!data)
	{
		kthread_exit();
		return;
	}
	struct rpcconnection_t* connection = (struct rpcconnection_t*)data;

	// Jailbreak this new thread
	oni_threadEscape(curthread, NULL);

	// Initialize all of the buffers
	WriteLog(LL_Debug, "rpcconnection_serverThread init buffers");
	memset(connection->buffer, 0, sizeof(connection->buffer));

	// Set the running status
	connection->isRunning = true;

	const uint32_t messageHeaderSize = sizeof(struct message_header_t);
	uint32_t dataReceived = 0;

	WriteLog(LL_Debug, "here");

	// While we have the server running
	while (connection->isRunning)
	{
		WriteLog(LL_Debug, "here");

		//WriteLog(LL_Debug, "rpcconnection_serverThread zeroing buffer");
		// Zero out the buffer
		memset(connection->buffer, 0, sizeof(connection->buffer));

		WriteLog(LL_Debug, "here");

		// If the connection socket has an error state
		if (connection->socket < 0)
		{
			WriteLog(LL_Error, "socket error: %d", connection->socket);
			goto cleanup;
		}

		WriteLog(LL_Debug, "here");

		//WriteLog(LL_Debug, "rpcconnection_serverThread begin recv");
		dataReceived = 0;

		// Create a new initial header
		struct message_header_t* header = (struct message_header_t*)kmalloc(messageHeaderSize);
		if (!header)
		{
			WriteLog(LL_Error, "could not allocate header");
			goto cleanup;
		}


		// Try to get the size of a full message
		int32_t recvSize = krecv(connection->socket, header, messageHeaderSize, 0);
		if (recvSize <= 0)
		{
			WriteLog(LL_Error, "%d recv returned %d.", connection->socket, recvSize);
			kfree(header, messageHeaderSize);
			goto cleanup;
		}

		WriteLog(LL_Debug, "here");
		// Increment the dataReceived
		dataReceived += recvSize;

		// Otherwise, try to get the entire full message
		while (dataReceived < messageHeaderSize)
		{
			//WriteLog(LL_Debug, "rpcconnection_serverThread recv more %d", dataReceived);
			uint32_t wantRecvSize = messageHeaderSize - dataReceived;

			recvSize = krecv(connection->socket, ((uint8_t*)header) + dataReceived, wantRecvSize, 0);
			if (recvSize <= 0)
			{
				WriteLog(LL_Error, "recv err returned %d.", recvSize);
				goto cleanup;
			}

			dataReceived += recvSize;
		}

		WriteLog(LL_Debug, "here");
		uint16_t payloadSize = header->payloadSize;
		uint64_t totalMessageSize = sizeof(*header) + payloadSize;

		// Check if we have an additional header
		if (header->payloadSize != 0)
		{
			// Hold the old reference, because this needs to be freed
			struct message_header_t* oldHeader = header;



			// Check to make sure we are within our buffer
			if (totalMessageSize > 0x4000 /* TODO: Change this to be well defined*/)
			{
				WriteLog(LL_Error, "payload size is larger than maximum buffer 0x4000");
				kfree(oldHeader, sizeof(*oldHeader));
				goto cleanup;
			}

			// Try to allocate a new buffer including space for our payload
			header = (struct message_header_t*)krealloc(header, totalMessageSize);
			if (!header)
			{
				WriteLog(LL_Error, "could not realloc message");
				kfree(oldHeader, sizeof(*oldHeader));
				goto cleanup;
			}

			// Zero out our payload area
			void* payloadStart = ((uint8_t*)header) + sizeof(*header);
			memset(payloadStart, 0, payloadSize);

			// Free the previous header allocation
			kfree(oldHeader, sizeof(*oldHeader));
		}

		// Check the message header magic
		if (header->magic != RPCMESSAGE_HEADER_MAGIC)
		{
			WriteLog(LL_Error, "invalid header got 0x%02x expected 0x%02x", header->magic, RPCMESSAGE_HEADER_MAGIC);
			goto cleanup;
		}
			
		WriteLog(LL_Debug, "here");

		// Recv the payload
		while (dataReceived < totalMessageSize)
		{
			uint64_t dataSizeRemaining = totalMessageSize - dataReceived;
			recvSize = krecv(connection->socket, ((uint8_t*)header) + dataReceived, dataSizeRemaining & 0xFFFFFFFF, 0);
			if (recvSize <= 0)
			{
				WriteLog(LL_Error, "could not recv all payload data");
				kfree(header, totalMessageSize);
				goto cleanup;
			}

			dataReceived += recvSize;
		}

		// Create a copy and free the original resource
		struct ref_t* allocation = ref_fromObject(header, totalMessageSize);
		if (!allocation)
		{
			WriteLog(LL_Error, "could not create a new reference from object");
			kfree(header, totalMessageSize);
			goto cleanup;
		}
		
		// Free our previous allocated message as we don't need it anymore
		kfree(header, totalMessageSize);
		header = NULL;

		WriteLog(LL_Debug, "dispatching message %p %p", gFramework->messageManager, allocation);

		// Now that we have the full message chunk, parse the category and the type and get it the fuck outa here
		messagemanager_sendRequest(allocation);

		ref_release(allocation);
	}

	
cleanup:
	connection->isRunning = false;
	if (connection->socket >= 0)
	{
		kshutdown(connection->socket, 2);
		kclose(connection->socket);
		connection->socket = -1;
	}

	if (connection->disconnect != NULL &&
		connection->server != NULL)
		connection->disconnect(connection->server, connection);
	
	kthread_exit();
}