
#include "utils/log.h"

#include <stdarg.h>
#include <stdio.h>

#include <chrono>
#include <iomanip>
#include <sstream>
#include <string>
#include <thread>

namespace {

using time_clock_t = std::chrono::time_point<std::chrono::system_clock>;

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
  time_clock_t now;
  std::thread::id thread_id;
  const char *filename;
  int line;
  const char *func_name;
};

const char *getEnumStr(LogLevel level) {
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

static LogLevel SKIP_LOG_LEVEL = kTrace;
static void (*EXPORT_CALLBACK)(LogLevel, const char *, int) = nullptr;

bool isSkipLog(const LogData &data) { return data.level < SKIP_LOG_LEVEL; }

void formatLog(const LogData &data, std::string &log) {
  // log head
  std::time_t now_time = std::chrono::system_clock::to_time_t(data.now);
  auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                data.now.time_since_epoch())
                .count() %
            1000;

  std::stringstream ss_h;
  // ISO 8601 data format
  struct tm buff;
  ss_h << "["
       << FORMAT_TM(LOCALTIME_S(&now_time, &buff), "(%z)%Y-%m-%d %H:%M:%S.")
       << std::setfill('0') << std::setw(3) << ms << "]"
       << "[" << data.thread_id << "]"
       << "[" << getEnumStr(data.level) << "]"
       << "[" << data.filename << ":" << data.line << "]"
       << "[" << data.func_name << "]"
       << " " << log.c_str();
  // log tail

  if (!(log.size() > 0 && *log.rbegin() == '\n')) {
    ss_h << ("\n");
  }

  log = ss_h.str();
}

void exportLog(const LogData &data, const std::string &log) {
  if (EXPORT_CALLBACK) {
    EXPORT_CALLBACK(data.level, log.c_str(), log.size());
    return;
  }
  auto out_stream = data.level >= kError ? stderr : stdout;
  fprintf(out_stream, "%s", log.c_str());
}

} // namespace

void setLogLevel(LogLevel level) { SKIP_LOG_LEVEL = level; }

void setExportCallback(void (*callback)(LogLevel, const char *, int)) {
  EXPORT_CALLBACK = callback;
}

void logPrint(LogLevel level, const char *filename, int line,
              const char *func_name, const char *fmt, ...) {

  LogData log_data = {};

  log_data.level = level;
  log_data.filename = filename;
  log_data.line = line;
  log_data.func_name = func_name;
  log_data.now = std::chrono::system_clock::now();
  log_data.thread_id = std::this_thread::get_id();

  if (isSkipLog(log_data)) {
    return;
  }

  // format the log
  va_list args;
  va_start(args, fmt);
  size_t size = vsnprintf(nullptr, 0, fmt, args);
  va_end(args);

  std::string log(size, 0);
  va_start(args, fmt);
  vsnprintf((char *)log.c_str(), log.size() + 1, fmt, args);
  va_end(args);

  formatLog(log_data, log);

  // export the log
  exportLog(log_data, log);
}
