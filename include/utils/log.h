
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

/**
  0       Emergency: system is unusable
  1       Alert: action must be taken immediately
  2       Critical: critical conditions
  3       Error: error conditions
  4       Warning: warning conditions
  5       Notice: normal but significant condition
  6       Informational: informational messages
  7       Debug: debug-level messages
*/
enum LogLevel {
  kEmergency,
  kAlert,
  kCritical,
  kError,
  kWarning,
  kNotice,
  kInfo,
  kDebug,
};

class source_location {
public:
  constexpr source_location(const char *name, int line, const char *f_name)
      : file_name_(name), file_line_(line), func_name_(f_name) {}

  constexpr size_t line() const noexcept { return (size_t)file_line_; }
  constexpr size_t column() const noexcept { return 0u; }
  constexpr const char *file_name() const noexcept { return file_name_; }
  constexpr const char *function_name() const noexcept { return func_name_; }

private:
  int file_line_;
  const char *file_name_;
  const char *func_name_;
};

// deprecated
#define LOG_TRACE(...) LOG_LEVEL_NOSD(kDebug, __VA_ARGS__)

#define LOG_DEBUG(...) LOG_LEVEL_NOSD(kDebug, __VA_ARGS__)
#define LOG_WARN(...) LOG_LEVEL_NOSD(kWarning, __VA_ARGS__)
#define LOG_INFO(...) LOG_LEVEL_NOSD(kInfo, __VA_ARGS__)
#define LOG_ERROR(...) LOG_LEVEL_NOSD(kError, __VA_ARGS__)

#define LOG_LEVEL_NOSD(level, ...)                                             \
  log_print(level, {__FILENAME__, __LINE__, __PRETTY_FUNCTION__},              \
            " " __VA_ARGS__)

// STRUCTURED-DATA and message type is "[" SD-ID *(SP SD-PARAM) "]" [SP MSG]
#define LOG_LEVEL_SD(level, ...)                                               \
  log_print(level, {__FILENAME__, __LINE__, __PRETTY_FUNCTION__}, __VA_ARGS__)

void log_print(LogLevel level, source_location const localtion, const char *fmt,
               ...);

void set_log_level(LogLevel level); // default no kDebug
void set_skip_funname(int skip);    // default is skip
void set_skip_filename(int skip);   // default no skip

void set_export_callback(void (*)(LogLevel, const char *, int));

#ifdef __cplusplus
}
#endif

#endif // UTILS_LOG_H
