
#ifndef PROACTOR_OPERATION_TCPSTREAMOP_H
#define PROACTOR_OPERATION_TCPSTREAMOP_H

#include <functional>

#include "proactor/Proactor.h"
#include "proactor/operation/detail/ConnectOp.h"
#include "proactor/operation/detail/RecvOp.h"
#include "proactor/operation/detail/SendOp.h"

class TcpStreamOp {
public:
  typedef std::function<void(const std::error_code &, size_t)> func_type;

  TcpStreamOp();
  explicit TcpStreamOp(Proactor *context);
  TcpStreamOp(Proactor *context, socket_type s);

  TcpStreamOp(const TcpStreamOp &);
  const TcpStreamOp &operator=(const TcpStreamOp &);

  void connect(const SocketAddr &addr, std::error_code &ec);
  void async_connect(const SocketAddr &addr, func_type f, std::error_code &ec);

  void async_read(char *buff, size_t buff_size, func_type f,
                  std::error_code &ec);
  void async_write(const char *buff, size_t buff_size, func_type f,
                   std::error_code &ec);

  size_t read(char *buff, size_t buff_size, std::error_code &ec);
  size_t write(const char *buff, size_t buff_size, std::error_code &ec);

  void close(std::error_code &ec);

  socket_type native_handle() const;

private:
  Proactor *ctx_;
  socket_type socket_;
  detail::ConnectOp connect_op_;
  detail::RecvOp recv_op_;
  detail::SendOp send_op_;

}; // class TcpListener

#endif // PROACTOR_OPERATION_TCPSTREAMOP_H
