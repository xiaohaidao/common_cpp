
#include "utils/log.h"

#include <stdarg.h>
#include <stdio.h>

#include <chrono>
#include <iomanip>
#include <sstream>
#include <string>
#include <thread>

namespace {

using TimeClockT = std::chrono::time_point<std::chrono::system_clock>;

#if defined(_WIN32)
#define LOCALTIME_S(timer, buf)                                                \
  [](const std::time_t *timers, struct tm *buff) {                             \
    localtime_s(buff, timers);                                                 \
    return buff;                                                               \
  }(timer, buf)
#else
#define LOCALTIME_S localtime_r
#endif

#if !defined(_MSC_VER) && __GNUC__ < 5
#define FORMAT_TM(tm, format)                                                  \
  ({                                                                           \
    char buf[32];                                                              \
    std::strftime(buf, 32, format, tm);                                        \
    buf;                                                                       \
  })
#else
#define FORMAT_TM std::put_time
#endif

struct LogData {
  LogLevel level;
  TimeClockT now;
  std::thread::id thread_id;
  const char *filename;
  int line;
  const char *func_name;
};

const char *get_enum_str(LogLevel level) {
  switch (level) {
  case kTrace:
    return "Trace";
    break;
  case kDebug:
    return "Debug";
    break;
  case kWarn:
    return "Warn";
    break;
  case kInfo:
    return "Info";
    break;
  case kError:
    return "Error";
    break;

  default:
    return "None";
    break;
  }
}

static LogLevel skip_log_level = kTrace;
static void (*export_callback)(LogLevel, const char *, int) = nullptr;

bool is_skip_log(const LogData &data) { return data.level < skip_log_level; }

void format_log(const LogData &data, std::string &log) {
  // log head
  std::time_t const now_time = std::chrono::system_clock::to_time_t(data.now);
  auto us = std::chrono::duration_cast<std::chrono::microseconds>(
                data.now.time_since_epoch())
                .count() %
            1000000;

  std::stringstream ss_h;
  // ISO 8601 data format
  struct tm buff;
  ss_h << "["
       << FORMAT_TM(LOCALTIME_S(&now_time, &buff), "(%z)%Y-%m-%d %H:%M:%S.")
       << std::setfill('0') << std::setw(6) << us << "]"
       << "[" << data.thread_id << "]"
       << "[" << get_enum_str(data.level) << "]"
       << "[" << data.filename << ":" << data.line << "]"
       << "[" << data.func_name << "]"
       << " " << log.c_str();
  // log tail

  if (!(log.size() > 0 && *log.rbegin() == '\n')) {
    ss_h << ("\n");
  }

  log = ss_h.str();
}

void export_log(const LogData &data, const std::string &log) {
  if (export_callback) {
    export_callback(data.level, log.c_str(), (int)log.size());
    return;
  }
  auto *out_stream = data.level >= kError ? stderr : stdout;
  fprintf(out_stream, "%s", log.c_str());
}

} // namespace

void set_log_level(LogLevel level) { skip_log_level = level; }

void set_export_callback(void (*callback)(LogLevel, const char *, int)) {
  export_callback = callback;
}

void log_print(LogLevel level, const char *filename, int line,
               const char *func_name, const char *fmt, ...) {

  LogData log_data = {};

  log_data.level = level;
  log_data.filename = filename;
  log_data.line = line;
  log_data.func_name = func_name;
  log_data.now = std::chrono::system_clock::now();
  log_data.thread_id = std::this_thread::get_id();

  if (is_skip_log(log_data)) {
    return;
  }

  // format the log
  va_list args;
  va_start(args, fmt);
  size_t const size = vsnprintf(nullptr, 0, fmt, args);
  va_end(args);

  std::string log(size, 0);
  va_start(args, fmt);
  vsnprintf((char *)log.c_str(), log.size() + 1, fmt, args);
  va_end(args);

  format_log(log_data, log);

  // export the log
  export_log(log_data, log);
}
