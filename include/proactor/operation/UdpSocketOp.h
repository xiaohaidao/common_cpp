
#ifndef PROACTOR_OPERATION_UDPSOCKETOP_H
#define PROACTOR_OPERATION_UDPSOCKETOP_H

#include <functional>

#include "proactor/Proactor.h"
#include "proactor/operation/detail/RecvFromOp.h"
#include "proactor/operation/detail/SendToOp.h"

class udp_socket_op {
public: //! construct
  using func_recv_type =
      std::function<void(const std::error_code &, size_t, const socket_addr &)>;
  using func_send_type = std::function<void(const std::error_code &, size_t)>;

  udp_socket_op();
  explicit udp_socket_op(proactor &context);
  udp_socket_op(proactor &context, socket_type s);

  udp_socket_op(const udp_socket_op &);
  udp_socket_op &operator=(const udp_socket_op &);

public: //! socket op
  void create(FamilyType family, std::error_code &ec);

  // The default family is ipv4
  void bind(const char *port_or_service, std::error_code &ec);
  void bind(const char *port_or_service, FamilyType family,
            std::error_code &ec);

  void close(std::error_code &ec);

public:
  void async_read(char *buff, size_t buff_size, const func_recv_type &f,
                  std::error_code &ec);
  void async_write(const char *buff, size_t buff_size, const socket_addr &to,
                   const func_send_type &f, std::error_code &ec);

  std::pair<size_t, socket_addr> recv_from(char *buff, size_t buff_size,
                                           std::error_code &ec);
  size_t send_to(const char *buff, size_t buff_size, const socket_addr &to,
                 std::error_code &ec);

  socket_type native() const;

private:
  proactor *ctx_{nullptr};
  socket_type socket_{-1};
  detail::recv_from_op recvfrom_op_;
  detail::send_to_op sendto_op_;
#ifdef __linux__
  socket_type write_socket_{-1};
#endif

}; // class udp_socket_op

#endif // PROACTOR_OPERATION_UDPSOCKETOP_H
