
#ifndef PROACTOR_OPERATION_UPDSOCKETOP_H
#define PROACTOR_OPERATION_UPDSOCKETOP_H

#include <functional>

#include "proactor/Proactor.h"
#include "proactor/operation/detail/RecvFromOp.h"
#include "proactor/operation/detail/SendToOp.h"

class UdpSocketOp {
public:
  typedef std::function<void(const std::error_code &, size_t,
                             const SocketAddr &)>
      func_recv_type;
  typedef std::function<void(const std::error_code &, size_t)> func_send_type;

  UdpSocketOp();
  explicit UdpSocketOp(Proactor *context);
  UdpSocketOp(Proactor *context, socket_type s);

  UdpSocketOp(const UdpSocketOp &);
  const UdpSocketOp &operator=(const UdpSocketOp &);

  void async_read(char *buff, size_t buff_size, func_recv_type f,
                  std::error_code &ec);
  void async_write(const char *buff, size_t buff_size, const SocketAddr &to,
                   func_send_type f, std::error_code &ec);

  std::pair<size_t, SocketAddr> recv_from(char *buff, size_t buff_size,
                                          std::error_code &ec);
  size_t send_to(const char *buff, size_t buff_size, const SocketAddr &to,
                 std::error_code &ec);

  void close(std::error_code &ec);

  socket_type native_handle() const;

private:
  Proactor *ctx_;
  socket_type socket_;
  detail::RecvFromOp recvfrom_op_;
  detail::SendToOp sendto_op_;

}; // class UdpSocketOp

#endif // PROACTOR_OPERATION_UPDSOCKETOP_H
