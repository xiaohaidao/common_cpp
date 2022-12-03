
#ifndef PROACTOR_OPERATION_TCPSTREAMOP_H
#define PROACTOR_OPERATION_TCPSTREAMOP_H

#include <functional>

#include "Proactor.h"
#include "operation/detail/RecvOp.h"
#include "operation/detail/SendOp.h"

class Proactor;

class TcpStreamOp {
public:
  typedef std::function<void(const std::error_code &, size_t)> func_type;

  explicit TcpStreamOp(Operation &context);
  TcpStreamOp(Operation &context, sockets::socket_type s);

  void connect(const sockets::SocketAddr &addr, std::error_code &ec);
  void async_connect(const sockets::SocketAddr &addr, std::error_code &ec);

  void async_read(char *buff, size_t buff_size, func_type f,
                  std::error_code &ec);
  void async_write(const char *buff, size_t buff_size, func_type f,
                   std::error_code &ec);

  size_t read(char *buff, size_t buff_size, std::error_code &ec);
  size_t write(const char *buff, size_t buff_size, std::error_code &ec);

  void close(std::error_code &ec);

  sockets::socket_type native_handle() const;

private:
  Proactor &ctx_;
  sockets::socket_type socket_;
  detail::RecvOp recv_op_;
  detail::SendOp send_op_;

}; // class TcpListener

#endif // PROACTOR_OPERATION_TCPSTREAMOP_H
