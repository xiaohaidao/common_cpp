
#ifdef __linux__

#include "process/Process.h"

#include <spawn.h>
#include <sys/wait.h>
#include <unistd.h>

#include "utils/error_code.h"
#include "utils/macro.h"

Process::Process() : child_handle_(0) {}

Process Process::call(const char *command, const std::vector<std::string> &argv,
                      std::error_code &ec) {

  std::vector<const char *> arg, env;
  for (auto const &i : argv) {
    arg.push_back(i.c_str());
  }
  arg.push_back(nullptr);

  Process sys;
  CHECK_EC(ec, sys);

  posix_spawn_file_actions_t file_actions = {};
  posix_spawn_file_actions_init(&file_actions);
  // posix_spawn_file_actions_addclose(&file_actions, STDOUT_FILENO);

  posix_spawnattr_t attr = {};
  posix_spawnattr_init(&attr);
  posix_spawnattr_setflags(&attr, POSIX_SPAWN_USEVFORK);

  pid_t pid = 0;
  if (posix_spawnp(&pid, command, &file_actions, &attr,
                   const_cast<char *const *>(arg.data()), nullptr) != 0) {
    ec = getErrorCode();
    return sys;
  }
  posix_spawnattr_destroy(&attr);
  posix_spawn_file_actions_destroy(&file_actions);

  if (!ec) {
    sys.child_handle_ = pid;
  }
  return sys;
}

Process Process::open(uint64_t pid, std::error_code &ec) {
  Process p;
  CHECK_EC(ec, p);
  p.child_handle_ = pid;
  return p;
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
    ec = getErrorCode();
  }
}

#endif // __linux__
