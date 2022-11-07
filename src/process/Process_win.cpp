
#ifdef WIN32

#include "process/Process.h"

#include "utils/error_code.h"
#include "utils/macro.h"

Process::Process() : child_handle_(0) {}

Process Process::call(const char *command, const std::vector<std::string> &argv,
                      std::error_code &ec) {

  std::string arg;
  for (auto const &i : argv) {
    arg.append(i);
    arg.append(" ");
  }

  Process sys;
  CHECK_EC(ec, sys);

  STARTUPINFO si;
  PROCESS_INFORMATION pi;

  ZeroMemory(&si, sizeof(si));
  si.cb = sizeof(si);
  ZeroMemory(&pi, sizeof(pi));
  if (!CreateProcess(command, // module name (use command line)
                     const_cast<LPSTR>(arg.c_str()), // Command line
                     NULL,             // Process handle not inheritable
                     NULL,             // Thread handle not inheritable
                     FALSE,            // Set handle inheritance to FALSE
                     CREATE_NO_WINDOW, // No creation flags
                     NULL,             // Use parent's environment block
                     NULL,             // Use parent's starting directory
                     &si,              // Pointer to STARTUPINFO structure
                     &pi) // Pointer to PROCESS_INFORMATION structure
  ) {
    ec = getErrorCode();
    return sys;
  }
  sys.child_handle_ = pi.hProcess;
  return sys;
}

Process Process::open(uint64_t pid, std::error_code &ec) {
  Process p;
  CHECK_EC(ec, p);
  HANDLE han = OpenProcess(PROCESS_ALL_ACCESS, false, pid);
  if (han == nullptr) {
    ec = getErrorCode();
    return p;
  }
  p.child_handle_ = han;
  return p;
}

bool Process::running(std::error_code &ec) {
  CHECK_EC(ec, false);

  DWORD re = WaitForSingleObject(child_handle_, 0);
  if (re == WAIT_FAILED) {
    ec = getErrorCode();
  }
  if (re != WAIT_TIMEOUT) {
    return false;
  }
  return true;
}

void Process::wait(std::error_code &ec) {
  CHECK_EC(ec, );

  if (WaitForSingleObject(child_handle_, INFINITE) == WAIT_FAILED) {
    ec = getErrorCode();
  }
}

void Process::terminate(std::error_code &ec) {
  CHECK_EC(ec, );
  if (!CloseHandle(child_handle_)) {
    ec = getErrorCode();
  }
}

#endif // WIN32
