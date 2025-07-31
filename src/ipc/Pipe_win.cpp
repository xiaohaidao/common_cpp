
#ifdef _WIN32

#include "ipc/Pipe.h"

#include <windows.h>

#include "utils/error_code.h"

namespace ipc {

pipe::pipe() : read_pipe_(0), write_pipe_(0), error_pipe_(0) {}

pipe pipe::std_in_out() {
  pipe re;
  re.read_pipe_ = GetStdHandle(STD_INPUT_HANDLE);
  re.write_pipe_ = GetStdHandle(STD_OUTPUT_HANDLE);
  re.error_pipe_ = GetStdHandle(STD_ERROR_HANDLE);
  return re;
}

pipe pipe::create(std::error_code &ec) {
  pipe re;

  SECURITY_ATTRIBUTES inherit;
  inherit.nLength = sizeof(SECURITY_ATTRIBUTES);
  inherit.bInheritHandle = TRUE;
  inherit.lpSecurityDescriptor = NULL;
  int const default_size = 0;
  if (!::CreatePipe(&re.read_pipe_, &re.write_pipe_, &inherit, default_size)) {
    ec = get_error_code();
  }
  return re;
}

size_t pipe::read(char *buff, size_t buff_size, std::error_code &ec) {
  DWORD num = 0;
  if (!::ReadFile(read_pipe_, buff, static_cast<DWORD>(buff_size), &num,
                  NULL)) {
    ec = get_error_code();
  }
  return num;
}

size_t pipe::write(const char *buff, size_t buff_size, std::error_code &ec) {
  DWORD num = 0;
  if (!::WriteFile(write_pipe_, buff, static_cast<DWORD>(buff_size), &num,
                   NULL)) {
    ec = get_error_code();
  }
  return num;
}

void pipe::close(std::error_code &ec) {
  if (!::CloseHandle(read_pipe_)) {
    ec = get_error_code();
  }
  if (!::CloseHandle(write_pipe_)) {
    ec = get_error_code();
  }
}

native_handle pipe::read_native() const { return read_pipe_; }

native_handle pipe::write_native() const { return write_pipe_; }

native_handle pipe::error_native() const { return error_pipe_; }

} // namespace ipc

#endif // _WIN32
