



#pragma once
#include <oni/utils/types.h>

// Plugin name length
#define PLUGIN_NAME_LEN		64

// Plugin description length
#define PLUGIN_DESC_LEN		256

/*
	plugin_t

	Plugin structure that all plugins should "inherit" from
*/
struct plugin_t
{
	// Name of the plugin
	const char* name;

	// Description of the plugin
	const char* description;

	// Plugin initialization prototype
	int32_t(*plugin_init)(void* arg);

	// Plugin close prototype
	int32_t(*plugin_close)();
};