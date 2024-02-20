
#ifndef IPC_PIPESTREAM_H
#define IPC_PIPESTREAM_H

#include <system_error>

namespace ipc {

#ifdef _WIN32
typedef void *native_handle;
#else
typedef int native_handle;
#endif

class PipeStream {
public:
  PipeStream();
#if defined(__linux__)
  PipeStream(native_handle native_handle);
#elif defined(_WIN32)
  PipeStream(native_handle native_handle, bool is_server);
#endif

  static PipeStream connect(const char *name_pipe, std::error_code &ec);

  // It can read and write by itself on Linux
  size_t read(char *buff, size_t buff_size, std::error_code &ec);
  size_t write(const char *buff, size_t buff_size, std::error_code &ec);

  void close(std::error_code &ec);

  native_handle native() const;

private:
  native_handle named_pipe_;

#ifdef _WIN32
  bool is_server_;
#else //__linux__
#endif
};

} // namespace ipc

#endif // IPC_PIPESTREAM_H
