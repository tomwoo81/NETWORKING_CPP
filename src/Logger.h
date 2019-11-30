#ifndef _LOGGER_H_
#define _LOGGER_H_

#ifndef __cplusplus // C Programme

#include <stdio.h>
#include "CommonTypes.h"

#define MODULE_ID_SOCKET 14

#ifndef FILENAME
#include <string.h>
#define FILENAME (NULL==strrchr(__FILE__, '\\')? \
        ((NULL==strrchr(__FILE__,'/'))?__FILE__: \
		(strrchr(__FILE__,'/')+1)):strrchr(__FILE__,'\\')+1)
#endif /* FILENAME */

#ifndef LINE
#define LINE __LINE__
#endif /* LINE */

#define LOG_DBG(moduleId, fmt, ...) \
printf(fmt, ##__VA_ARGS__)

#define LOG_INFO(moduleId, fmt, ...) \
printf(fmt, ##__VA_ARGS__)

#define LOG_WARN(moduleId, fmt, ...) \
printf(fmt, ##__VA_ARGS__)

#define LOG_ERROR(moduleId, fmt, ...) \
printf(fmt, ##__VA_ARGS__)

#define LOG_BUFFER(moduleId, pBuff, buffLen) \
LogBufferToStream((S32)moduleId, 1, FILENAME, LINE, (const char*)pBuff, buffLen, 16)

void LogToStream(S32 moduleId, S32 logLevel,
		const char *file, S32 line,
		const char *fmt, ...);

void LogBufferToStream(S32 moduleId, S32 logLevel,
		const char *file, S32 line,
		const char* pBuff, U32 buffLen,
		U32 numPerLine);

#else // C++ Programme

#endif // __cplusplus

#endif // _LOGGER_H_

/* End of File */
