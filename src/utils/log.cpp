
// reference: https://datatracker.ietf.org/doc/html/rfc5424

#include "utils/log.h"

#ifdef _WIN32
#include <winsock.h>
#else
#include <unistd.h>
#endif

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
  size_t line;
  const char *func_name;
};

const char *get_enum_str(LogLevel level) {
  switch (level) {
  case kDebug:
    return "Debug";
  case kWarning:
    return "Warning";
  case kInfo:
    return "Info";
  case kError:
    return "Error";
  case kEmergency:
    return "Emergency";
  case kAlert:
    return "Alert";
  case kCritical:
    return "Critical";
  case kNotice:
    return "Notice";
  default:
    return "None";
  }
}

static LogLevel skip_log_level = kDebug;
static int skip_funname = 1;
static int skip_filename = 0;
static void (*export_callback)(LogLevel, const char *, int) = nullptr;

bool is_skip_log(const LogData &data) { return data.level >= skip_log_level; }

int format_head(const LogData &data, char *str, size_t size) {
  // log head
  std::time_t const now_time = std::chrono::system_clock::to_time_t(data.now);
  int64_t const us = std::chrono::duration_cast<std::chrono::microseconds>(
                         data.now.time_since_epoch())
                         .count() %
                     1000000;

  // RFC5424:
  // SYSLOG-MSG: HEADER SP STRUCTURED-DATA [SP MSG]
  // HEADER: PRI VERSION SP TIMESTAMP SP HOSTNAME SP APP-NAME SP PROCID SP MSGID
  // STRUCTURED-DATA: NILVALUE / 1*SD-ELEMENT
  // SD-ELEMENT: "[" SD-ID *(SP SD-PARAM) "]"
  // message: [SP MSG]
  int n = 0;
  // format PRI VERSION SP
  n += snprintf(str + n, size - n, "<%d>1 ",
                16 * 8 + data.level); // 16 = local use 0  (local0)

  // format TIMESTAMP SP
  // ISO 8601 data format
  // YY-MM-DDThh:mm::ss.000000Z+00:00
  {
    std::stringstream ss;
    struct tm buff;
    ss << FORMAT_TM(LOCALTIME_S(&now_time, &buff), "%Y-%m-%dT%H:%M:%S");
    n += snprintf(str + n, size - n, "%s", ss.str().c_str());
    n += snprintf(str + n, size - n, ".%06lld", us);
    int min_zone = 0;
    ss.str(std::string());
    ss << FORMAT_TM(LOCALTIME_S(&now_time, &buff), "%z");
#ifdef _MSC_VER
    sscanf_s(ss.str().c_str(), "%d", &min_zone);
#else
    sscanf(ss.str().c_str(), "%d", &min_zone);
#endif
    min_zone = min_zone / 100 * 60 + min_zone % 100;
    n += snprintf(str + n, size - n, "%s%02d:%02d ", min_zone < 0 ? "-" : "+",
                  abs(min_zone) / 60, abs(min_zone) % 60);
  }
  // format HOSTNAME SP
  {
    char hostname[32] = {};
    if (::gethostname(hostname, sizeof(hostname))) {
      n += snprintf(str + n, size - n, "- ");
    } else {
      n += snprintf(str + n, size - n, "%s ", hostname);
    }
  }

#ifdef _WIN32
  // format APP-NAME SP
  {
    char pid_name[512] = {};
    DWORD pid_size = sizeof(pid_name);
    if (QueryFullProcessImageName(GetCurrentProcess(), 0, pid_name,
                                  &pid_size)) {
      n += snprintf(str + n, size - n, "%s ", SUB_WIN_PATH(pid_name));
    } else {
      n += snprintf(str + n, size - n, "- ");
    }
  }

  // format PROCID SP
  n += snprintf(str + n, size - n, "%d ", GetCurrentProcessId());
#else
  // format APP-NAME SP
  {
    FILE *f = ::fopen("/proc/self/cmdline", "r");
    if (f != NULL) {
      char pid_name[512] = {};
      fgets(pid_name, sizeof(pid_name), f); // get line
      n += snprintf(str + n, size - n, "%s ", SUB_UNIX_PATH(pid_name));
      ::fclose(f);
    } else {
      n += snprintf(str + n, size - n, "- ");
    }
  }

  // format PROCID SP
  n += snprintf(str + n, size - n, "%d ", getpid());
#endif

  // format MSGID SP
  n += snprintf(str + n, size - n, "- ");

  // format STRUCTURED-DATA {
  // format thread id
  std::stringstream ss;
  ss << data.thread_id;
  n += snprintf(str + n, size - n, "[Tid@%s", ss.str().c_str());

  // format level
  n += snprintf(str + n, size - n, " Level=\"%s\"", get_enum_str(data.level));

  // format file and line
  if (!skip_filename) {
    n += snprintf(str + n, size - n, " File=\"%s:%zu\"", data.filename,
                  data.line);
  }

  // format function name
  if (!skip_funname) {
    n += snprintf(str + n, size - n, " fun=\"%s\"", data.func_name);
  }
  n += snprintf(str + n, size - n, "]");
  // format STRUCTURED-DATA }

  return n;
}

void export_log(const LogData &data, const char *log, size_t size) {
  if (export_callback) {
    export_callback(data.level, log, (int)size);
    return;
  }
  auto *out_stream = data.level >= kError ? stderr : stdout;
  fprintf(out_stream, "%.*s", (int)size, log);
}

} // namespace

void set_log_level(LogLevel level) { skip_log_level = level; }
void set_skip_funname(int skip) { skip_funname = !!skip; }
void set_skip_filename(int skip) { skip_filename = !!skip; }

void set_export_callback(void (*callback)(LogLevel, const char *, int)) {
  export_callback = callback;
}

void log_print(LogLevel level, source_location const location, const char *fmt,
               ...) {

  LogData log_data = {};

  log_data.level = level;
  log_data.filename = location.file_name();
  log_data.line = location.line();
  log_data.func_name = location.function_name();
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

  std::string log;
  char str_data[1024];
  int const head_size = format_head(log_data, str_data, sizeof(str_data));
  size_t all_size = head_size + size + 2; // terminal: '\n' + '\0'

  char *log_begin = str_data + head_size;
  char *data_begin = str_data;
  size_t log_size = sizeof(str_data) - head_size;
  if (all_size > sizeof(str_data)) {
    log.resize(all_size, 0);
    data_begin = &log[0];
    memcpy(data_begin, str_data, head_size);
    log_size = log.size() - head_size;
    log_begin = data_begin + head_size;
  }
  va_start(args, fmt);
  vsnprintf(log_begin, log_size, fmt, args);
  va_end(args);
  if (data_begin[all_size - 3] == '\n') {
    --all_size;
  } else {
    data_begin[all_size - 2] = '\n';
    data_begin[all_size - 1] = '\0';
  }

  // export the log
  export_log(log_data, data_begin, all_size);
}
