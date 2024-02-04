
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

  static PipeStream connect(const char *name_pipe, std::error_code &ec);

  // It can read and write by itself on Linux
  size_t read(char *buff, size_t buff_size, std::error_code &ec);
  size_t write(const char *buff, size_t buff_size, std::error_code &ec);

  void close(std::error_code &ec);

  native_handle native() const;

private:
  native_handle named_pipe_;

#ifdef _WIN32
#else //__linux__
  std::string name_;
#endif
};

} // namespace ipc

#endif // IPC_PIPESTREAM_H
