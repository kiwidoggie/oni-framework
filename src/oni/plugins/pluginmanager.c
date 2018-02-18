#include <oni/plugins/pluginmanager.h>
#include <oni/plugins/plugin.h>

#include <oni/utils/lock.h>
#include <oni/utils/log/logger.h>

void pluginmanager_init(struct pluginmanager_t* manager)
{
	if (!manager)
		return;

	// Zero out our plugin table
	for (uint32_t i = 0; i < PLUGINMANAGER_MAX_PLUGINS; ++i)
		manager->plugins[i] = 0;

	manager->lock.lock = 0;
}

int32_t pluginmanager_findFreePluginIndex(struct pluginmanager_t* manager)
{
	if (!manager)
		return -1;

	int32_t pluginIndex = -1;

	spin_lock(&manager->lock);
	for (uint32_t i = 0; i < PLUGINMANAGER_MAX_PLUGINS; ++i)
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
	for (uint32_t i = 0; i < PLUGINMANAGER_MAX_PLUGINS; ++i)
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
		return 0;
	}


	int32_t pluginIndex = pluginmanager_findFreePluginIndex(manager);
	if (pluginIndex == -1)
	{
		WriteLog(LL_Error, "no free indices");
		return 0;
	}


	int32_t pluginExists = 0;

	// Verify that this plugin isn't already loaded
	spin_lock(&manager->lock);
	for (uint32_t i = 0; i < PLUGINMANAGER_MAX_PLUGINS; ++i)
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
		return 0;
	}

	// Assign our plugin
	manager->plugins[pluginIndex] = plugin;

	// TODO: Make auto-loading configurable
	WriteLog(LL_Debug, "loading plugin: %s", plugin->name);

	int32_t initResult = plugin->plugin_init(0);
	if (!initResult)
		return 0;

	return 1;
}

int32_t pluginmanager_unregisterPlugin(struct pluginmanager_t* manager, struct plugin_t* plugin)
{
	for (uint64_t i = 0; i < PLUGINMANAGER_MAX_PLUGINS; ++i)
	{
		if (manager->plugins[i] == plugin)
		{
			pluginmanager_unregisterPlugin(manager, manager->plugins[i]);
			return 1;
		}
	}

	return 0;
}

void pluginmanager_shutdown(struct pluginmanager_t* manager)
{
	spin_lock(&manager->lock);
	for (uint64_t i = 0; i < PLUGINMANAGER_MAX_PLUGINS; ++i)
	{
		if (manager->plugins[i] == 0)
			continue;

		pluginmanager_unregisterPlugin(manager, manager->plugins[i]);
	}
	spin_unlock(&manager->lock);
}