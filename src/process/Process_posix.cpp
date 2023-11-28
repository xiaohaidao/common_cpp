
#ifdef __linux__

#include "process/Process.h"

#include <spawn.h>
#include <sys/wait.h>
#include <unistd.h>

#include "ipc/Pipe.h"
#include "utils/error_code.h"
#include "utils/macro.h"

extern char **environ;

Process::Process() : child_handle_(0) {}

Process Process::call(const char *command,
                      const std::vector<const char *> &argv,
                      std::error_code &ec) {

  ipc::Pipe pipe = ipc::Pipe::std_in_out();
  return call(command, argv, pipe, ec);
}

Process Process::call(const char *command,
                      const std::vector<const char *> &argv,
                      const ipc::Pipe &pipe, std::error_code &ec) {

  std::vector<const char *> arg;
  arg.push_back(command);
  for (auto const &i : argv) {
    if (i) {
      arg.push_back(i);
    }
  }
  arg.push_back(nullptr);

  Process sys;

  posix_spawn_file_actions_t file_actions = {};
  posix_spawn_file_actions_init(&file_actions);
  // posix_spawn_file_actions_addclose(&file_actions, STDOUT_FILENO);
  if (posix_spawn_file_actions_adddup2(&file_actions, STDIN_FILENO,
                                       pipe.read_native())) {

    ec = get_error_code();
  }
  posix_spawn_file_actions_adddup2(&file_actions, STDOUT_FILENO,
                                   pipe.write_native());
  posix_spawn_file_actions_adddup2(&file_actions, STDERR_FILENO,
                                   pipe.error_native());

  posix_spawnattr_t attr = {};
  posix_spawnattr_init(&attr);
  posix_spawnattr_setflags(&attr, POSIX_SPAWN_USEVFORK);

  pid_t pid = 0;
  if (posix_spawnp(&pid, command, &file_actions, &attr,
                   const_cast<char *const *>(arg.data()), environ) != 0) {

    ec = get_error_code();
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
  p.child_handle_ = pid;
  return p;
}

bool Process::running(std::error_code &ec) {
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
    return false;
  }
  return true;
}

int Process::wait(std::error_code &ec) {
  int status = -1;
  if (::waitpid(child_handle_, &status, 0) == -1) {
    int e = errno;
    if (e != ECHILD) {
      ec = {e, std::system_category()};
    }
  }
  return WEXITSTATUS(status);
}

void Process::terminate(std::error_code &ec) {
  if (::kill(child_handle_, SIGKILL) == -1) {
    ec = get_error_code();
  }
}

#endif // __linux__
