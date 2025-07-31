// Copyright (C) 2022 All rights reserved.
// Email: oxox0@qq.com. Created in 202207

#ifndef SOCKETS_TCPLISTENER_H
#define SOCKETS_TCPLISTENER_H

#include "sockets/TcpStream.h"

class tcp_listener {
public:
  tcp_listener();
  explicit tcp_listener(const socket_type &s);

  // The default family is ipv4, bind and listen
  static tcp_listener bind(const char *port_or_service, std::error_code &ec);
  static tcp_listener bind(const char *port_or_service, FamilyType family,
                           std::error_code &ec);

  // The default family is ipv4, just bind
  static tcp_stream bind_port(const char *port_or_service, std::error_code &ec);
  static tcp_stream bind_port(const char *port_or_service, FamilyType family,
                              std::error_code &ec);

  std::pair<tcp_stream, socket_addr> accept(std::error_code &ec);

  void close(std::error_code &ec);

  void set_read_timeout(size_t timeout_ms, std::error_code &ec);
  size_t read_timeout(std::error_code &ec) const;

  socket_type native() const;

private:
  socket_type socket_{-1};

}; // class tcp_listener

#endif // SOCKETS_TCPLISTENER_H
