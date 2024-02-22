
#ifdef _WIN32

#include "ipc/PipeListener.h"

#include <cstring>
#include <windows.h>

#include "utils/error_code.h"

namespace ipc {

PipeListener::PipeListener() : name_{}, named_pipe_(nullptr) {}

PipeListener PipeListener::create(const char *name_pipe, std::error_code &ec) {
  PipeListener re;
  constexpr int kBufSize = 4096;
  char buff_name[256];
  snprintf(buff_name, sizeof(buff_name), "%s%s", "\\\\.\\pipe\\", name_pipe);
  HANDLE server = ::CreateNamedPipe(
      buff_name,                                 // pipe name
      PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED, // read/write access
      PIPE_TYPE_MESSAGE |                        // message type pipe
          PIPE_READMODE_MESSAGE |                // message-read mode
          PIPE_WAIT,                             // blocking mode
      PIPE_UNLIMITED_INSTANCES,                  // max. instances
      kBufSize,                                  // output buffer size
      kBufSize,                                  // input buffer size
      0,                                         // client time-out
      NULL);
  if (server == INVALID_HANDLE_VALUE) {
    ec = get_error_code();
    return re;
  }

  re.named_pipe_ = server;
  snprintf(re.name_, sizeof(re.name_), "%s", name_pipe);
  return re;
}
void PipeListener::create(std::error_code &ec) {
  *this = PipeListener::create(name_, ec);
}

PipeStream PipeListener::accept(std::error_code &ec) {
  if (named_pipe_ == NULL) {
    (*this) = create(name_, ec);
  }
  PipeStream re(named_pipe_, true);
  if (!::ConnectNamedPipe(named_pipe_, NULL)) {
    std::error_code const re_ec = get_error_code();
    if (re_ec.value() != ERROR_IO_PENDING &&
        re_ec.value() != ERROR_PIPE_CONNECTED) {
      ec = re_ec;
      return PipeStream();
    }
  }
  named_pipe_ = NULL;
  return re;
}

void PipeListener::remove(std::error_code &ec) {
  if (named_pipe_ != NULL && !::CloseHandle(named_pipe_)) {
    ec = get_error_code();
  }
}

native_handle PipeListener::native() const { return named_pipe_; }

} // namespace ipc

#endif // _WIN32
