
#ifndef PROCESS_PROCESS_H
#define PROCESS_PROCESS_H

#include <string>
#include <system_error>
#include <vector>

class Process {
public:
#ifdef WIN32
  typedef void *native_handle;
#else  // WIN32
  typedef pid_t native_handle;
#endif // WIN32
  Process();

  static Process call(const char *command, const std::vector<std::string> &argv,
                      std::error_code &ec);

  static Process open(uint64_t pid, std::error_code &ec);

  bool running(std::error_code &ec);

  // just wait child process
  void wait(std::error_code &ec);
  // bool waitFor(size_t timeout_ms, std::error_code &ec);

  void terminate(std::error_code &ec);

private:
  native_handle child_handle_;
};

#endif // PROCESS_PROCESS_H
