
#ifdef _WIN32

#include "ipc/Pipe.h"

#include <windows.h>

#include "utils/error_code.h"

namespace ipc {

Pipe::Pipe()
    : read_pipe_(0), write_pipe_(0), error_pipe_(0), is_server_(false) {}

Pipe Pipe::std_in_out() {
  Pipe re;
  re.read_pipe_ = GetStdHandle(STD_INPUT_HANDLE);
  re.write_pipe_ = GetStdHandle(STD_OUTPUT_HANDLE);
  re.error_pipe_ = GetStdHandle(STD_ERROR_HANDLE);
  return re;
}

Pipe Pipe::create(std::error_code &ec) {
  Pipe re;

  SECURITY_ATTRIBUTES inherit;
  inherit.nLength = sizeof(SECURITY_ATTRIBUTES);
  inherit.bInheritHandle = TRUE;
  inherit.lpSecurityDescriptor = NULL;
  int default_size = 0;
  if (!::CreatePipe(&re.read_pipe_, &re.write_pipe_, &inherit, default_size)) {
    ec = getErrorCode();
  }
  return re;
}

Pipe Pipe::create(const char *name_pipe, std::error_code &ec) {
  Pipe re;
  constexpr int BUF_SIZE = 512;
  char buff_name[256] = "\\\\.\\pipe\\";
  memcpy(buff_name + strlen(buff_name), name_pipe, strlen(name_pipe));
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
    ec = getErrorCode();
    return re;
  }

  re.read_pipe_ = server;
  re.write_pipe_ = server;
  re.error_pipe_ = re.read_pipe_;
  re.is_server_ = true;
  return re;
}

Pipe Pipe::connect(const char *name_pipe, std::error_code &ec) {
  Pipe re;
  char buff_name[256] = "\\\\.\\pipe\\";
  memcpy(buff_name + strlen(buff_name), name_pipe, strlen(name_pipe));
  HANDLE client = ::CreateFile(buff_name, GENERIC_WRITE | GENERIC_READ, 0, NULL,
                               OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);
  if (client == INVALID_HANDLE_VALUE) {
    ec = getErrorCode();
    return re;
  }
  re.read_pipe_ = client;
  re.write_pipe_ = client;
  re.error_pipe_ = re.read_pipe_;
  return re;
}

size_t Pipe::read(char *buff, size_t buff_size, std::error_code &ec) {
  if (is_server_) {
    if (!::ConnectNamedPipe(read_pipe_, NULL)) {
      std::error_code re_ec = getErrorCode();
      if (re_ec.value() != ERROR_IO_PENDING &&
          re_ec.value() != ERROR_PIPE_CONNECTED) {
        ec = re_ec;
      }
    }
  }
  DWORD num = 0;
  if (!::ReadFile(read_pipe_, buff, buff_size, &num, NULL)) {
    ec = getErrorCode();
  }
  return num;
}

size_t Pipe::write(const char *buff, size_t buff_size, std::error_code &ec) {
  if (is_server_) {
    if (!::ConnectNamedPipe(write_pipe_, NULL)) {
      std::error_code re_ec = getErrorCode();
      if (re_ec.value() != ERROR_IO_PENDING &&
          re_ec.value() != ERROR_PIPE_CONNECTED) {
        ec = re_ec;
      }
    }
  }
  DWORD num = 0;
  if (!::WriteFile(write_pipe_, buff, buff_size, &num, NULL)) {
    ec = getErrorCode();
  }
  return num;
}

void Pipe::close(std::error_code &ec) {
  if (read_pipe_ == write_pipe_) {
    if (is_server_ && !::DisconnectNamedPipe(read_pipe_)) {
      ec = getErrorCode();
    }
  }
  if (!::CloseHandle(read_pipe_)) {
    ec = getErrorCode();
  }
  if (read_pipe_ != write_pipe_) {
    if (!::CloseHandle(write_pipe_)) {
      ec = getErrorCode();
    }
  }
}

native_handle Pipe::read_native() const { return read_pipe_; }

native_handle Pipe::write_native() const { return write_pipe_; }

native_handle Pipe::error_native() const { return error_pipe_; }

} // namespace ipc

#endif // _WIN32
