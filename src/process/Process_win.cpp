
#ifdef _WIN32

#include "process/Process.h"

#include "ipc/Pipe.h"
#include "utils/error_code.h"
#include "utils/macro.h"

Process::Process() : child_handle_(0) {}

Process Process::call(const char *command,
                      const std::vector<const char *> &argv,
                      const ipc::Pipe &pipe, std::error_code &ec) {

  std::string arg;
  arg.append(command);
  for (auto const &i : argv) {
    arg.append(" ");
    arg.append(i);
  }

  Process sys;

  STARTUPINFO si;
  PROCESS_INFORMATION pi;

  ZeroMemory(&si, sizeof(si));
  si.cb = sizeof(si);
  si.hStdError = pipe.error_native();
  si.hStdOutput = pipe.write_native();
  si.hStdInput = pipe.read_native();
  ZeroMemory(&pi, sizeof(pi));
  if (!CreateProcess(NULL, // module name (use command line)
                     const_cast<LPSTR>(arg.data()), // Command line
                     NULL, // Process handle not inheritable
                     NULL, // Thread handle not inheritable
                     TRUE, // Set handle inheritance to FALSE
                     0,    // No creation flags
                     NULL, // Use parent's environment block
                     NULL, // Use parent's starting directory
                     &si,  // Pointer to STARTUPINFO structure
                     &pi)  // Pointer to PROCESS_INFORMATION structure
  ) {
    ec = getErrorCode();
    return sys;
  }
  sys.child_handle_ = pi.hProcess;
  return sys;
}

Process Process::call(const char *command,
                      const std::vector<const char *> &argv,
                      std::error_code &ec) {

  ipc::Pipe pipe = ipc::Pipe::std_in_out();
  return call(command, argv, pipe, ec);
}

Process Process::open(uint64_t pid, std::error_code &ec) {
  Process p;
  HANDLE han = OpenProcess(PROCESS_ALL_ACCESS, false, static_cast<DWORD>(pid));
  if (han == nullptr) {
    ec = getErrorCode();
    return p;
  }
  p.child_handle_ = han;
  return p;
}

bool Process::running(std::error_code &ec) {
  DWORD re = WaitForSingleObject(child_handle_, 0);
  if (re == WAIT_FAILED) {
    ec = getErrorCode();
  }
  if (re != WAIT_TIMEOUT) {
    return false;
  }
  return true;
}

int Process::wait(std::error_code &ec) {
  int status = -1;
  if (WaitForSingleObject(child_handle_, INFINITE) == WAIT_FAILED) {
    ec = getErrorCode();
  }
  if (!ec) {
    if (GetExitCodeProcess(child_handle_, (DWORD *)&status) == 0) {
      ec = getErrorCode();
    }
    // STILL_ACTIVE == exit_code // Still running
  }
  return status;
}

void Process::terminate(std::error_code &ec) {
  if (!TerminateProcess(child_handle_, -1)) {
    ec = getErrorCode();
  }
}

#endif // _WIN32
