#pragma once

struct messagemanager_t;
struct logger_t;

/*
	oniframework_t

	Core framework
*/
struct oniframework_t
{
	// Message manager for the framework
	struct messagemanager_t* messageManager;

	// Logger for the framework
	struct logger_t* logger;
};