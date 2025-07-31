
#ifndef PROCESS_PROCESS_H
#define PROCESS_PROCESS_H

#include <cstdint>
#include <system_error>
#include <vector>

namespace ipc {
class pipe;
} // namespace ipc

class process {
public:
#ifdef _WIN32
  using native_handle = void *;
#else  // _WIN32
  using native_handle = pid_t;
#endif // _WIN32
  process();

  static process call(const char *command,
                      const std::vector<const char *> &argv,
                      const ipc::pipe &pipe, std::error_code &ec);
  static process call(const char *command,
                      const std::vector<const char *> &argv,
                      std::error_code &ec);

  static process open(uint64_t pid, std::error_code &ec);

  bool running(std::error_code &ec);

  // just wait child process
  int wait(std::error_code &ec);
  // bool waitFor(size_t timeout_ms, std::error_code &ec);

  void terminate(std::error_code &ec);

private:
  native_handle child_handle_{0};
};

#endif // PROCESS_PROCESS_H
