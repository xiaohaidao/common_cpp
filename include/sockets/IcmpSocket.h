
#ifndef SOCKETS_ICMPSOCKET_H
#define SOCKETS_ICMPSOCKET_H

#include "sockets/SocketAddr.h"

class IcmpSocket {
public:
  IcmpSocket();
  explicit IcmpSocket(const socket_type &s);

  static IcmpSocket create(FamilyType family, std::error_code &ec);

  // The default family is ipv4
  static IcmpSocket bind(const char *port_or_service, std::error_code &ec);
  static IcmpSocket bind(const char *port_or_service, FamilyType family,
                         std::error_code &ec);

  void close(std::error_code &ec);

  void set_read_timeout(size_t timeout_ms, std::error_code &ec);
  void set_write_timeout(size_t timeout_ms, std::error_code &ec);
  size_t read_timeout(std::error_code &ec) const;
  size_t write_timeout(std::error_code &ec) const;

  // return receive size
  std::pair<size_t, SocketAddr> recv_from(char *buf, size_t buf_size,
                                          std::error_code &ec);

  // buf_size >= data_size + 8(sizeof(struct icmphdr))
  // data will be copied to buf
  size_t send_to(char *buf, const char *data, size_t data_size,
                 const SocketAddr &to, std::error_code &ec);

  socket_type native_handle() const;

private:
  socket_type socket_;

#ifdef _WIN32
  size_t read_timeout_;
  size_t send_timeout_;
#endif // _WIN32

}; // class IcmpSocket

#endif // SOCKETS_ICMPSOCKET_H
