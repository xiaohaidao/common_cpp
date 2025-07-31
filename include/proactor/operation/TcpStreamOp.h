
#ifndef PROACTOR_OPERATION_TCPSTREAMOP_H
#define PROACTOR_OPERATION_TCPSTREAMOP_H

#include <functional>

#include "proactor/Proactor.h"
#include "proactor/operation/detail/ConnectOp.h"
#include "proactor/operation/detail/RecvOp.h"
#include "proactor/operation/detail/SendOp.h"

class tcp_stream_op {
public:
  using func_type = std::function<void(const std::error_code &, size_t)>;

  explicit tcp_stream_op(proactor *context);
  tcp_stream_op(proactor *context, socket_type s);

  tcp_stream_op(const tcp_stream_op &);
  tcp_stream_op &operator=(const tcp_stream_op &);

  void connect(const socket_addr &addr, std::error_code &ec);
  void async_connect(const socket_addr &addr, const func_type &f,
                     std::error_code &ec);

  void async_read(char *buff, size_t buff_size, const func_type &f,
                  std::error_code &ec);
  void async_write(const char *buff, size_t buff_size, const func_type &f,
                   std::error_code &ec);

  size_t read(char *buff, size_t buff_size, std::error_code &ec);
  size_t write(const char *buff, size_t buff_size, std::error_code &ec);

  void shutdown(std::error_code &ec);
  void close(std::error_code &ec);

  socket_type native() const;

private:
  proactor *ctx_{nullptr};
  socket_type socket_{-1};
  detail::connect_op connect_op_;
  detail::recv_op recv_op_;
  detail::send_op send_op_;
#ifdef __linux__
  socket_type write_socket_{-1};
#endif

}; // class tcp_stream_op

#endif // PROACTOR_OPERATION_TCPSTREAMOP_H
