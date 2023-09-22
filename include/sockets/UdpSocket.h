// Copyright (C) 2022 All rights reserved.
// Email: oxox0@qq.com. Created in 202207

#ifndef SOCKETS_UDPSOCKET_H
#define SOCKETS_UDPSOCKET_H

#include "sockets/SocketAddr.h"

class UdpSocket {
public:
  UdpSocket();
  explicit UdpSocket(const socket_type &s);

  static UdpSocket create(FamilyType family, std::error_code &ec);

  // The default family is ipv4
  static UdpSocket bind(const char *port_or_service, std::error_code &ec);
  static UdpSocket bind(const char *port_or_service, FamilyType family,
                        std::error_code &ec);

  void connected(const SocketAddr &addr, std::error_code &ec);
  void close(std::error_code &ec);

  void set_read_timeout(size_t timeout_ms, std::error_code &ec);
  void set_write_timeout(size_t timeout_ms, std::error_code &ec);
  size_t read_timeout(std::error_code &ec) const;
  size_t write_timeout(std::error_code &ec) const;

  // return receive size
  std::pair<int, SocketAddr> recv_from(char *buf, size_t buf_size,
                                       std::error_code &ec);

  // return send size
  int send_to(const char *buf, size_t buf_size, const SocketAddr &to,
              std::error_code &ec);

  void set_broadcast(bool enable, std::error_code &ec);
  bool broadcast(std::error_code &ec);

  void joint_multicast(const SocketAddr &multicast, std::error_code &ec);
  void leave_multicast(const SocketAddr &multicast, std::error_code &ec);
  void set_multicast_loop(bool enable, std::error_code &ec);
  bool multicast_loop(std::error_code &ec);
  void set_multicast_ttl(int ttl, std::error_code &ec);
  int multicast_ttl(std::error_code &ec);

  void joint_multicast_v6(const SocketAddr &multicast, std::error_code &ec);
  void leave_multicast_v6(const SocketAddr &multicast, std::error_code &ec);
  void set_multicast_loop_v6(bool enable, std::error_code &ec);
  bool multicast_loop_v6(std::error_code &ec);

  socket_type native_handle() const;

private:
  socket_type socket_;

}; // class UdpSocket

#endif // SOCKETS_UDPSOCKET_H
