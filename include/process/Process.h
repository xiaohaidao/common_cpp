
#ifndef PROCESS_PROCESS_H
#define PROCESS_PROCESS_H

#include <cstdint>
#include <system_error>
#include <vector>

namespace ipc {
class Pipe;
}

class Process {
public:
#ifdef _WIN32
  typedef void *native_handle;
#else  // _WIN32
  typedef pid_t native_handle;
#endif // _WIN32
  Process();

  static Process call(const char *command,
                      const std::vector<const char *> &argv,
                      const ipc::Pipe &pipe, std::error_code &ec);
  static Process call(const char *command,
                      const std::vector<const char *> &argv,
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
