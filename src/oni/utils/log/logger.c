#include <oni/utils/log/logger.h>
#include <stdarg.h>
#include <oni/utils/kdlsym.h>
#include <oni/utils/sys_wrappers.h>

void logger_init(struct logger_t* logger)
{
	if (!logger)
		return;

	logger->logLevel = LL_Debug;
	logger->logHandle = -1;

	kmemset(logger->buffer, 0, sizeof(logger->buffer));
	kmemset(logger->finalBuffer, 0, sizeof(logger->finalBuffer));

	spin_init(&logger->lock);
}

void logger_writelog(struct logger_t* logger, enum LogLevels level, const char* function, int line, const char* fmt, ...)
{
	if (!logger)
		return;

	// If the logger is set not to output anything, skip everything
	if (logger->logLevel <= LL_None)
		return;

	if (level > logger->logLevel)
		return;

	int(*snprintf)(char *str, size_t size, const char *format, ...) = kdlsym(snprintf);
	int(*vsnprintf)(char *str, size_t size, const char *format, va_list ap) = kdlsym(vsnprintf);
	void(*printf)(char *format, ...) = kdlsym(printf);

	spin_lock(&logger->lock);

	// Zero out the buffer
	kmemset(logger->buffer, 0, sizeof(logger->buffer));
	kmemset(logger->finalBuffer, 0, sizeof(logger->finalBuffer));

	va_list args;
	va_start(args, fmt);
	vsnprintf(logger->buffer, sizeof(logger->buffer), fmt, args);
	va_end(args);

	const char* levelString = "None";
	const char* levelColor = KNRM;
	switch (level)
	{
	case LL_Info:
		levelString = "Info";
		levelColor = KGRN;
		break;
	case LL_Warn:
		levelString = "Warn";
		levelColor = KYEL;
		break;
	case LL_Error:
		levelString = "Error";
		levelColor = KRED;
		break;
	case LL_Debug:
		levelString = "Debug";
		levelColor = KMAG;
		break;
	case LL_None:
	default:
		levelString = "None";
		levelColor = KNRM;
		break;
	}

	snprintf(logger->finalBuffer, sizeof(logger->finalBuffer), "%s[%s] %s:%d : %s %s\n", levelColor, levelString, function, line, logger->buffer, KNRM);
	printf(logger->finalBuffer);

	spin_unlock(&logger->lock);
}