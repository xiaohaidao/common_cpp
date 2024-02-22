
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

#define __PRETTY_FUNCTION__ __FUNCSIG__
#endif

enum LogLevel {
  kTrace,
  kDebug,
  kInfo,
  kWarn,
  kError,
};

#define LOG_TRACE(...)                                                         \
  log_print(kTrace, __FILENAME__, __LINE__, __PRETTY_FUNCTION__, __VA_ARGS__)

#define LOG_DEBUG(...)                                                         \
  log_print(kDebug, __FILENAME__, __LINE__, __PRETTY_FUNCTION__, __VA_ARGS__)

#define LOG_WARN(...)                                                          \
  log_print(kWarn, __FILENAME__, __LINE__, __PRETTY_FUNCTION__, __VA_ARGS__)

#define LOG_INFO(...)                                                          \
  log_print(kInfo, __FILENAME__, __LINE__, __PRETTY_FUNCTION__, __VA_ARGS__)

#define LOG_ERROR(...)                                                         \
  log_print(kError, __FILENAME__, __LINE__, __PRETTY_FUNCTION__, __VA_ARGS__)

void log_print(LogLevel level, const char *filename, int line,
               const char *func_name, const char *fmt, ...);

void set_log_level(LogLevel level);

void set_export_callback(void (*)(LogLevel, const char *, int));

#ifdef __cplusplus
}
#endif

#endif // UTILS_LOG_H
