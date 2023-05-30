
#ifndef UTILS_LOG_H
#define UTILS_LOG_H

#ifdef __cplusplus
extern "C" {
#endif

#include <string.h>

#ifndef _MSC_VER
#define __FILENAME__                                                           \
  (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#else
#define __FILENAME__                                                           \
  (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)
#endif

enum LogLevel {
  kTrace,
  kDebug,
  kInfo,
  kWarn,
  kError,
};

#define LOG_TRACE(...)                                                         \
  logPrint(kTrace, __FILENAME__, __LINE__, __FUNCTION__, __VA_ARGS__)

#define LOG_DEBUG(...)                                                         \
  logPrint(kDebug, __FILENAME__, __LINE__, __FUNCTION__, __VA_ARGS__)

#define LOG_WARN(...)                                                          \
  logPrint(kWarn, __FILENAME__, __LINE__, __FUNCTION__, __VA_ARGS__)

#define LOG_INFO(...)                                                          \
  logPrint(kInfo, __FILENAME__, __LINE__, __FUNCTION__, __VA_ARGS__)

#define LOG_ERROR(...)                                                         \
  logPrint(kError, __FILENAME__, __LINE__, __FUNCTION__, __VA_ARGS__)

void logPrint(LogLevel level, const char *filename, int line,
              const char *func_name, const char *fmt, ...);

void setLogLevel(LogLevel level);

void setExportCallback(void (*)(LogLevel, const char *, int));

#ifdef __cplusplus
}
#endif

#endif // UTILS_LOG_H
