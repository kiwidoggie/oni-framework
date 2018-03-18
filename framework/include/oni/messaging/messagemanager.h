



#pragma once
#include <oni/utils/types.h>
#include <oni/utils/lock.h>

#define RPCDISPATCHER_MAX_CATEGORIES	32

struct message_t;
struct rpclistener_t;
struct messagecategory_t;
struct allocation_t;

struct messagemanager_t
{
	struct messagecategory_t* categories[RPCDISPATCHER_MAX_CATEGORIES];

	struct lock_t lock;
};

void messagemanager_init(struct messagemanager_t* dispatcher);
void messagemanager_sendErrorMessage(struct messagemanager_t* dispatcher, struct allocation_t* ref, int32_t error);
void messagemanager_sendSuccessMessage(struct messagemanager_t* dispatcher, struct allocation_t* ref);

int32_t messagemanager_findFreeCategoryIndex(struct messagemanager_t* dispatcher);
uint32_t messagemanager_freeCategoryCount(struct messagemanager_t* dispatcher);
struct messagecategory_t* messagemanager_getCategory(struct messagemanager_t* dispatcher, uint32_t categoryId);
int32_t messagemanager_registerCallback(struct messagemanager_t* dispatcher, uint32_t callbackCategory, uint32_t callbackType, void* callback);

void messagemanager_sendMessage(struct messagemanager_t* dispatcher, struct allocation_t* message);