
#ifndef PROACTOR_OPERATION_TCPLISTENEROP_H
#define PROACTOR_OPERATION_TCPLISTENEROP_H

#include "proactor/operation/TcpStreamOp.h"
#include "proactor/operation/detail/AcceptOp.h"

class TcpListenerOp {
public:
  typedef std::function<void(const std::error_code &,
                             std::pair<TcpStreamOp, sockets::SocketAddr>)>
      func_type;

  TcpListenerOp();
  explicit TcpListenerOp(Proactor &context);
  TcpListenerOp(Proactor &context, sockets::socket_type s);

  void bind(const char *port_or_service, std::error_code &ec);

  std::pair<TcpStreamOp, sockets::SocketAddr> accept(std::error_code &ec);
  void async_accept(func_type f, std::error_code &ec);

  sockets::socket_type native_handle() const;

  void close(std::error_code &ec);

private:
  Proactor *ctx_;
  sockets::socket_type socket_;
  // Buff buff_;
  detail::AcceptOp accept_op_;

}; // class TcpListener

#endif // PROACTOR_OPERATION_TCPLISTENEROP_H
