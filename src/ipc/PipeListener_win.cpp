
#ifdef _WIN32

#include "ipc/PipeListener.h"

#include <cstring>
#include <windows.h>

#include "utils/error_code.h"

namespace ipc {

pipe_listener::pipe_listener() : name_{}, named_pipe_(nullptr) {}

pipe_listener pipe_listener::create(const char *name_pipe,
                                    std::error_code &ec) {
  pipe_listener re;
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
void pipe_listener::create(std::error_code &ec) {
  *this = pipe_listener::create(name_, ec);
}

pipe_stream pipe_listener::accept(std::error_code &ec) {
  if (named_pipe_ == NULL) {
    (*this) = create(name_, ec);
  }
  pipe_stream re(named_pipe_, true);
  if (!::ConnectNamedPipe(named_pipe_, NULL)) {
    std::error_code const re_ec = get_error_code();
    if (re_ec.value() != ERROR_IO_PENDING &&
        re_ec.value() != ERROR_PIPE_CONNECTED) {
      ec = re_ec;
      return pipe_stream();
    }
  }
  named_pipe_ = NULL;
  return re;
}

void pipe_listener::remove(std::error_code &ec) {
  if (named_pipe_ != NULL && !::CloseHandle(named_pipe_)) {
    ec = get_error_code();
  }
}

native_handle pipe_listener::native() const { return named_pipe_; }

} // namespace ipc

#endif // _WIN32
