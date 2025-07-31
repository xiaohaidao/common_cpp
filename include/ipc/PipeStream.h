
#ifndef IPC_PIPESTREAM_H
#define IPC_PIPESTREAM_H

#include <system_error>

namespace ipc {

#ifdef _WIN32
using native_handle = void *;
#else
using native_handle = int;
#endif

class pipe_stream {
public:
  pipe_stream();
#if defined(__linux__)
  pipe_stream(native_handle native_handle);
#elif defined(_WIN32)
  pipe_stream(native_handle native_handle, bool is_server);
#endif

  static pipe_stream connect(const char *name_pipe, std::error_code &ec);

  // It can read and write by itself on Linux
  size_t read(char *buff, size_t buff_size, std::error_code &ec);
  size_t write(const char *buff, size_t buff_size, std::error_code &ec);

  void close(std::error_code &ec);

  native_handle native() const;

private:
  native_handle named_pipe_{0};

#ifdef _WIN32
  bool is_server_;
#else //__linux__
#endif
};

} // namespace ipc

#endif // IPC_PIPESTREAM_H
