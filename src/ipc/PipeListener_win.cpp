
#ifdef _WIN32

#include "ipc/PipeListener.h"

#include <windows.h>

#include "utils/error_code.h"

namespace ipc {

PipeListener::PipeListener() : named_pipe_(nullptr) {}

PipeListener PipeListener::create(const char *name_pipe, std::error_code &ec) {
  PipeListener re;
  constexpr int BUF_SIZE = 4096;
  char buff_name[256];
  snprintf(buff_name, sizeof(buff_name), "%s%s", "\\\\.\\pipe\\", name_pipe);
  HANDLE server = ::CreateNamedPipe(
      buff_name,                                 // pipe name
      PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED, // read/write access
      PIPE_TYPE_MESSAGE |                        // message type pipe
          PIPE_READMODE_MESSAGE |                // message-read mode
          PIPE_WAIT,                             // blocking mode
      PIPE_UNLIMITED_INSTANCES,                  // max. instances
      BUF_SIZE,                                  // output buffer size
      BUF_SIZE,                                  // input buffer size
      0,                                         // client time-out
      NULL);
  if (server == INVALID_HANDLE_VALUE) {
    ec = get_error_code();
    return re;
  }

  re.named_pipe_ = server;
  return re;
}

void PipeListener::accept(std::error_code &ec) {
  if (!::ConnectNamedPipe(named_pipe_, NULL)) {
    std::error_code re_ec = get_error_code();
    if (re_ec.value() != ERROR_IO_PENDING &&
        re_ec.value() != ERROR_PIPE_CONNECTED) {
      ec = re_ec;
    }
  }
}

size_t PipeListener::read(char *buff, size_t buff_size, std::error_code &ec) {
  DWORD num = 0;
  if (!::ReadFile(named_pipe_, buff, static_cast<DWORD>(buff_size), &num,
                  NULL)) {
    ec = get_error_code();
  }
  return num;
}

size_t PipeListener::write(const char *buff, size_t buff_size,
                           std::error_code &ec) {
  DWORD num = 0;
  if (!::WriteFile(named_pipe_, buff, static_cast<DWORD>(buff_size), &num,
                   NULL)) {
    ec = get_error_code();
  }
  return num;
}

void PipeListener::close(std::error_code &ec) {
  if (!::DisconnectNamedPipe(named_pipe_)) {
    ec = get_error_code();
  }
}

void PipeListener::remove(std::error_code &ec) {
  if (!::CloseHandle(named_pipe_)) {
    ec = get_error_code();
  }
}

native_handle PipeListener::native() const { return named_pipe_; }

} // namespace ipc

#endif // _WIN32
