// Copyright (C) 2022 All rights reserved.
// Email: oxox0@qq.com. Created in 202207

#ifndef SOCKETS_TCPLISTENER_H
#define SOCKETS_TCPLISTENER_H

#include "sockets/TcpStream.h"

class TcpListener {
public:
  TcpListener();
  explicit TcpListener(const socket_type &s);

  // The default family is ipv4, bind and listen
  static TcpListener bind(const char *port_or_service, std::error_code &ec);
  static TcpListener bind(const char *port_or_service, FamilyType family,
                          std::error_code &ec);

  // The default family is ipv4, just bind
  static TcpStream bind_port(const char *port_or_service, std::error_code &ec);
  static TcpStream bind_port(const char *port_or_service, FamilyType family,
                             std::error_code &ec);

  std::pair<TcpStream, SocketAddr> accept(std::error_code &ec);

  void close(std::error_code &ec);

  void set_read_timeout(size_t timeout_ms, std::error_code &ec);
  size_t read_timeout(std::error_code &ec) const;

  socket_type native_handle() const;

private:
  socket_type socket_;

#ifdef _WIN32
  size_t read_timeout_;
  size_t send_timeout_;
#endif // _WIN32

}; // class TcpListener

#endif // SOCKETS_TCPLISTENER_H
