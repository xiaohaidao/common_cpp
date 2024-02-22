// Copyright (C) 2022 All rights reserved.
// Email: oxox0@qq.com. Created in 202207

#ifndef SOCKETS_TCPSTREAM_H
#define SOCKETS_TCPSTREAM_H

#include "sockets/SocketAddr.h"

class TcpStream {
public:
  TcpStream();
  explicit TcpStream(const socket_type &s);

  static TcpStream connect(const SocketAddr &addr, std::error_code &ec);

  void connected(const SocketAddr &addr, std::error_code &ec);

  void shutdown(std::error_code &ec);

  void close(std::error_code &ec);

  void set_read_timeout(size_t timeout_ms, std::error_code &ec);
  void set_write_timeout(size_t timeout_ms, std::error_code &ec);
  size_t read_timeout(std::error_code &ec) const;
  size_t write_timeout(std::error_code &ec) const;

  // return read size
  int read(char *buf, size_t buf_size, std::error_code &ec);

  // return write size
  int write(const char *buf, size_t buf_size, std::error_code &ec);

  socket_type native() const;

private:
  friend class TcpListener;
  socket_type socket_;

}; // class TcpStream

#endif // SOCKETS_TCPSTREAM_H
