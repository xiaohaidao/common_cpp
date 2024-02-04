
#ifndef IPC_PIPE_H
#define IPC_PIPE_H

#include <system_error>

namespace ipc {

#ifdef _WIN32
typedef void *native_handle;
#else
typedef int native_handle;
#endif

class Pipe {
public:
  Pipe();

  static Pipe std_in_out();
  static Pipe create(std::error_code &ec);

  size_t read(char *buff, size_t buff_size, std::error_code &ec);
  size_t write(const char *buff, size_t buff_size, std::error_code &ec);

  void close(std::error_code &ec);

  native_handle read_native() const;
  native_handle write_native() const;
  native_handle error_native() const;

private:
  native_handle read_pipe_;
  native_handle write_pipe_;
  native_handle error_pipe_;
};

} // namespace ipc

#endif // IPC_PIPE_H
