
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

bool isSkipLog(const LogData &data) { return false; }

void formatLog(const LogData &data, std::string &log) {
  // log head
  std::time_t now_time = std::chrono::system_clock::to_time_t(data.now);
  auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                data.now.time_since_epoch())
                .count() %
            1000;

  std::stringstream ss_h;
  // ISO 8601 data format
  ss_h << "[" << std::put_time(localtime(&now_time), "(%z)%F %T.") << ms << "]"
       << "[" << data.thread_id << "]"
       << "[" << getEnumStr(data.level) << "]"
       << "[" << data.filename << ":" << data.line << "]"
       << "[" << data.func_name << "]"
       << " "
       << log.c_str()
       // log tail
       << ("\n");

  log = ss_h.str();
}

void exportLog(const std::string &log) { printf("%s", log.c_str()); }

} // namespace

void log_print(LogLevel level, const char *filename, int line,
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

  std::string log(size + 1, 0);
  va_start(args, fmt);
  vsnprintf((char *)log.c_str(), log.size(), fmt, args);
  va_end(args);
  log.resize(size);

  formatLog(log_data, log);

  // export the log
  exportLog(log);
}
