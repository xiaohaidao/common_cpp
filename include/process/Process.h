
#ifndef PROCESS_PROCESS_H
#define PROCESS_PROCESS_H

#include <string>
#include <system_error>
#include <vector>

class Process {
public:
  Process();
  explicit Process(pid_t pid);

  /// @example ```
  /// char *newargv[] = {"hello", "world", NULL };
  /// char *newenviron[] = { NULL };
  /// std::error_code ec;
  /// Process::call("echo", newargv, newenviron, ec);
  /// ```
  static Process call(const char *command, char const *const argv[],
                      char const *const envp[], std::error_code &ec);

  static Process call(const char *command, const std::vector<std::string> &argv,
                      const std::vector<std::string> &envp,
                      std::error_code &ec);

  bool running(std::error_code &ec);

  // just wait child process
  void wait(std::error_code &ec);
  // bool waitFor(size_t timeout_ms, std::error_code &ec);

  void terminate(std::error_code &ec);

private:
  pid_t child_handle_;
};

#endif // PROCESS_PROCESS_H
