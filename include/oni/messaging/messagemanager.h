



#pragma once
#include <oni/utils/types.h>
#include <sys/param.h>
#include <sys/lock.h>
#include <sys/mutex.h>

#define RPCDISPATCHER_MAX_CATEGORIES	32

struct message_header_t;
struct rpclistener_t;
struct messagecategory_t;
struct ref_t;

struct messagemanager_t
{
	struct messagecategory_t* categories[RPCDISPATCHER_MAX_CATEGORIES];

	struct mtx lock;
};

void messagemanager_init(struct messagemanager_t* manager);
//void messagemanager_sendErrorMessage(struct messagemanager_t* manager, struct allocation_t* msg, int32_t error);
//void messagemanager_sendSuccessMessage(struct messagemanager_t* manager, struct allocation_t* msg);

int32_t messagemanager_findFreeCategoryIndex(struct messagemanager_t* manager);
uint32_t messagemanager_freeCategoryCount(struct messagemanager_t* manager);
struct messagecategory_t* messagemanager_getCategory(struct messagemanager_t* manager, uint32_t categoryId);
int32_t messagemanager_registerCallback(struct messagemanager_t* manager, uint32_t callbackCategory, uint32_t callbackType, void* callback);
int32_t messagemanager_unregisterCallback(struct messagemanager_t* manager, int32_t callbackCategory, int32_t callbackType, void* callback);

//void messagemanager_sendMessage(struct messagemanager_t* manager, struct ref_t* msg);


void messagemanager_sendRequestLocal(struct ref_t* message);
void messagemanager_sendRequest(struct ref_t* message);

void messagemanager_pluginSendRequest(struct messagemanager_t* messageManager, struct ref_t* message);

void messagemanager_sendResponseLocal(struct ref_t* message, int32_t error);
void messagemanager_sendResponse(struct ref_t* message, int32_t error);

void messagemanager_pluginSendResponse(struct messagemanager_t* messageManager, struct ref_t* message);

void* message_getData(struct message_header_t* message);