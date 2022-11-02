
#ifdef __linux__

#include "process/Process.h"

#include <sys/wait.h>
#include <unistd.h>

#include "utils/macro.h"

Process::Process() : child_handle_(0) {}

Process::Process(pid_t pid) : child_handle_(pid) {}

Process Process::call(const char *command, char const *const argv[],
                      char const *const envp[], std::error_code &ec) {
  Process sys;
  CHECK_EC(ec, sys);

  pid_t pid = ::vfork();
  if (pid == -1) {
    ec = {errno, std::system_category()};
    return sys;
  } else if (pid == 0) {

    ::execve(command, const_cast<char *const *>(argv),
             const_cast<char *const *>(envp));
    ::_exit(EXIT_FAILURE);
  }

  sys.child_handle_ = pid;
  return sys;
}

Process Process::call(const char *command, const std::vector<std::string> &argv,
                      const std::vector<std::string> &envp,
                      std::error_code &ec) {

  std::vector<const char *> arg, env;
  for (auto const &i : argv) {
    arg.push_back(i.c_str());
  }
  arg.push_back(nullptr);

  for (auto const &i : envp) {
    env.push_back(i.c_str());
  }
  env.push_back(nullptr);

  return Process::call(command, const_cast<const char *const *>(arg.data()),
                       const_cast<const char *const *>(env.data()), ec);
}

bool Process::running(std::error_code &ec) {
  CHECK_EC(ec, false);

  int status = -1;
  if (::waitpid(child_handle_, &status, WNOHANG) == -1) {
    int e = errno;
    if (::kill(child_handle_, 0) != -1) {
      return true;
    }
    if (e != ECHILD) {
      ec = {e, std::system_category()};
    }
    return false;
  }
  if (WIFEXITED(status) || WIFSIGNALED(status) || WIFSTOPPED(status)) {
    printf("stop singal %d\n", status);
    return false;
  }
  return true;
}

void Process::wait(std::error_code &ec) {
  CHECK_EC(ec, );

  int status = -1;
  if (::waitpid(child_handle_, &status, 0) == -1) {
    int e = errno;
    if (e != ECHILD) {
      ec = {e, std::system_category()};
    }
  }
}

void Process::terminate(std::error_code &ec) {
  CHECK_EC(ec, );
  if (::kill(child_handle_, SIGKILL) == -1) {
    ec = {errno, std::system_category()};
  }
}

#endif // __linux__
