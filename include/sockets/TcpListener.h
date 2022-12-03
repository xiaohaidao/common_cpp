// Copyright (C) 2022 All rights reserved.
// Email: oxox0@qq.com. Created in 202207

#ifndef SOCKETS_TCPLISTENER_H
#define SOCKETS_TCPLISTENER_H

#include <map>

#include "sockets/TcpStream.h"

namespace sockets {

class TcpListener {
public:
  TcpListener();
  explicit TcpListener(const socket_type &s);

  static TcpListener bind(const char *port_or_service, std::error_code &ec);

  std::pair<TcpStream, SocketAddr> accept(std::error_code &ec);

  void close(std::error_code &ec);

  socket_type native_handle() const;

private:
  socket_type socket_;

}; // class TcpListener

} // namespace sockets

#endif // SOCKETS_TCPLISTENER_H
