
#ifndef PROACTOR_OPERATION_IPCSTREAMOP_H
#define PROACTOR_OPERATION_IPCSTREAMOP_H

#include "ipc/PipeStream.h"
#include "proactor/operation/TcpStreamOp.h"
#include "proactor/operation/detail/ReadOp.h"
#include "proactor/operation/detail/WriteOp.h"

class ipc_stream_op {
public:
  using func_type = std::function<void(const std::error_code &, size_t)>;

  explicit ipc_stream_op(proactor *context);

#if defined(_WIN32)
  explicit ipc_stream_op(proactor *context, const ipc::pipe_stream &pipe);
#elif defined(__linux__)
  explicit ipc_stream_op(const tcp_stream_op &tcp);
#endif
  ipc_stream_op(const ipc_stream_op &);
  ipc_stream_op &operator=(const ipc_stream_op &);

  void connect(const char *name, std::error_code &ec);

  void async_read(char *buff, size_t buff_size, const func_type &f,
                  std::error_code &ec);
  void async_write(const char *buff, size_t buff_size, const func_type &f,
                   std::error_code &ec);
  size_t read(char *buff, size_t size, std::error_code &ec);
  size_t write(const char *buff, size_t size, std::error_code &ec);

  void close(std::error_code &ec);

  native_handle native() const;

private:
#if defined(_WIN32)
  proactor *ctx_;

  detail::ReadOp read_;
  detail::WriteOp write_;
  ipc::pipe_stream pipe_;
#elif defined(__linux__)
  tcp_stream_op tcp_;
#endif

}; // class Ipc

#endif // PROACTOR_OPERATION_IPCSTREAMOP_H
