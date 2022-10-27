
#ifndef UTILS_LOG_H
#define UTILS_LOG_H

#ifdef __cplusplus
extern "C" {
#endif

enum LogLevel {
  kTrace,
  kDebug,
  kWarn,
  kInfo,
  kError,
};

#define LOG_TRACE(...)                                                         \
  log_print(kTrace, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)

#define LOG_DEBUG(...)                                                         \
  log_print(kDebug, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)

#define LOG_WARN(...)                                                          \
  log_print(kWarn, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)

#define LOG_INFO(...)                                                          \
  log_print(kInfo, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)

#define LOG_ERROR(...)                                                         \
  log_print(kError, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)

void log_print(LogLevel level, const char *filename, int line,
               const char *func_name, const char *fmt, ...);

#ifdef __cplusplus
}
#endif

#endif // UTILS_LOG_H
