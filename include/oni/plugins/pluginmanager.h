#pragma once
#include <oni/utils/types.h>
#include <oni/utils/lock.h>

#include <oni/config.h>

struct plugin_t;

/*
	pluginmanager_t

	Plugin manager for adding, removing, and interacting with plugins
*/
struct pluginmanager_t
{
	// Plugins list
	struct plugin_t* plugins[PLUGINMANAGER_MAX_PLUGINS];

	// Lock
	struct lock_t lock;
};

// Pluginmanager initialization
void pluginmanager_init(struct pluginmanager_t* manager);

// Pluginmanager shutdown
void pluginmanager_shutdown(struct pluginmanager_t* manager);

// Finds the free plugin index to use
int32_t pluginmanager_findFreePluginIndex(struct pluginmanager_t*);

// Gets the current plugin count
uint32_t pluginmanager_pluginCount(struct pluginmanager_t*);

// Register plugin
int32_t pluginmanager_registerPlugin(struct pluginmanager_t*, struct plugin_t*);

// Unregister plugin
int32_t pluginmanager_unregisterPlugin(struct pluginmanager_t*, struct plugin_t*);