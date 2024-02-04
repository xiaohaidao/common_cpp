
#ifdef _WIN32

#include "ipc/PipeStream.h"

#include <windows.h>

#include "utils/error_code.h"

namespace ipc {

PipeStream::PipeStream() : named_pipe_(0) {}

PipeStream PipeStream::connect(const char *name_pipe, std::error_code &ec) {
  PipeStream re;
  char buff_name[256];
  snprintf(buff_name, sizeof(buff_name), "%s%s", "\\\\.\\pipe\\", name_pipe);
  HANDLE client = ::CreateFile(buff_name, GENERIC_WRITE | GENERIC_READ, 0, NULL,
                               OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);
  if (client == INVALID_HANDLE_VALUE) {
    ec = get_error_code();
    return re;
  }
  re.named_pipe_ = client;
  return re;
}

size_t PipeStream::read(char *buff, size_t buff_size, std::error_code &ec) {
  DWORD num = 0;
  if (!::ReadFile(named_pipe_, buff, static_cast<DWORD>(buff_size), &num,
                  NULL)) {
    ec = get_error_code();
  }
  return num;
}

size_t PipeStream::write(const char *buff, size_t buff_size,
                         std::error_code &ec) {
  DWORD num = 0;
  if (!::WriteFile(named_pipe_, buff, static_cast<DWORD>(buff_size), &num,
                   NULL)) {
    ec = get_error_code();
  }
  return num;
}

void PipeStream::close(std::error_code &ec) {
  if (!::CloseHandle(named_pipe_)) {
    ec = get_error_code();
  }
}

native_handle PipeStream::native() const { return named_pipe_; }

} // namespace ipc

#endif // _WIN32
