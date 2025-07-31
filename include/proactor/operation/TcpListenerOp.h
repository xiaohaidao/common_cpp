
#ifndef PROACTOR_OPERATION_TCPLISTENEROP_H
#define PROACTOR_OPERATION_TCPLISTENEROP_H

#include "proactor/operation/TcpStreamOp.h"
#include "proactor/operation/detail/AcceptOp.h"

class tcp_listener_op {
public:
  using func_type = std::function<void(
      const std::error_code &, const std::pair<tcp_stream_op, socket_addr> &)>;

  tcp_listener_op();
  explicit tcp_listener_op(proactor &context);
  tcp_listener_op(proactor &context, socket_type s);

  tcp_listener_op(const tcp_listener_op &) noexcept;
  tcp_listener_op &operator=(const tcp_listener_op &) noexcept;

  void bind(const char *port_or_service, std::error_code &ec);
  void bind(const char *port_or_service, FamilyType family,
            std::error_code &ec);

  std::pair<tcp_stream_op, socket_addr> accept(std::error_code &ec);
  void async_accept(const func_type &f, std::error_code &ec);

  socket_type native() const;

  void close(std::error_code &ec);

private:
  proactor *ctx_{nullptr};
  socket_type socket_;
  detail::accept_op accept_op_;

}; // class tcp_listener

#endif // PROACTOR_OPERATION_TCPLISTENEROP_H
