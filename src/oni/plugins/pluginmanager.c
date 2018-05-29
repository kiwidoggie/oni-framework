#include <oni/plugins/pluginmanager.h>
#include <oni/plugins/plugin.h>

#include <oni/utils/lock.h>
#include <oni/utils/logger.h>

void pluginmanager_init(struct pluginmanager_t* manager)
{
	if (!manager)
		return;

	// Zero out our plugin table
	for (uint32_t i = 0; i < ARRAYSIZE(manager->plugins); ++i)
		manager->plugins[i] = NULL;

	spin_init(&manager->lock);
}

int32_t pluginmanager_findFreePluginIndex(struct pluginmanager_t* manager)
{
	if (!manager)
		return -1;

	int32_t pluginIndex = -1;

	spin_lock(&manager->lock);
	for (uint32_t i = 0; i < ARRAYSIZE(manager->plugins); ++i)
	{
		if (!manager->plugins[i])
		{
			pluginIndex = i;
			break;
		}
	}
	spin_unlock(&manager->lock);

	return pluginIndex;
}

uint32_t pluginmanager_pluginCount(struct pluginmanager_t* manager)
{
	if (!manager)
		return 0;

	uint32_t count = 0;

	spin_lock(&manager->lock);
	for (uint32_t i = 0; i < ARRAYSIZE(manager->plugins); ++i)
	{
		if (manager->plugins[i])
			count++;
	}
	spin_unlock(&manager->lock);

	return count;
}

int32_t pluginmanager_registerPlugin(struct pluginmanager_t* manager, struct plugin_t* plugin)
{
	WriteLog(LL_Info, "registering '%s' plugin", plugin->name);

	if (!manager)
	{
		WriteLog(LL_Error, "no manager");
		return false;
	}


	int32_t pluginIndex = pluginmanager_findFreePluginIndex(manager);
	if (pluginIndex == -1)
	{
		WriteLog(LL_Error, "no free indices");
		return false;
	}


	int32_t pluginExists = 0;

	// Verify that this plugin isn't already loaded
	spin_lock(&manager->lock);
	for (uint32_t i = 0; i < ARRAYSIZE(manager->plugins); ++i)
	{
		if (manager->plugins[i] == plugin)
		{
			pluginExists = 1;
			break;
		}
	}
	spin_unlock(&manager->lock);

	if (pluginExists)
	{
		WriteLog(LL_Error, "plugin exists");
		return false;
	}

	// Assign our plugin
	manager->plugins[pluginIndex] = plugin;

	// TODO: Make auto-loading configurable
	WriteLog(LL_Debug, "loading plugin: %s", plugin->name);

	// Bugcheck?
	uint8_t pluginLoadResult = plugin->plugin_load ? plugin->plugin_load(plugin) : false;
	if (!pluginLoadResult)
		return false;

	return true;
}

int32_t pluginmanager_unregisterPlugin(struct pluginmanager_t* manager, struct plugin_t* plugin)
{
	spin_lock(&manager->lock);
	for (uint64_t i = 0; i < ARRAYSIZE(manager->plugins); ++i)
	{
		if (manager->plugins[i] == plugin)
		{
			// Unload the plugin
			if (plugin->plugin_unload) // Bugcheck?
				plugin->plugin_unload(plugin);
			manager->plugins[i] = NULL;
			spin_unlock(&manager->lock);
			return true;
		}
	}
	spin_unlock(&manager->lock);
	return false;
}

void pluginmanager_shutdown(struct pluginmanager_t* manager)
{
	for (uint64_t i = 0; i < ARRAYSIZE(manager->plugins); ++i)
	{
		if (manager->plugins[i] == NULL)
			continue;

		pluginmanager_unregisterPlugin(manager, manager->plugins[i]);
	}
}