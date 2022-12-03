// Copyright (C) 2022 All rights reserved.
// Email: oxox0@qq.com. Created in 202207

#ifndef SOCKETS_UDPSOCKET_H
#define SOCKETS_UDPSOCKET_H

#include <map>
#include <vector>

#include "sockets/SocketAddr.h"

namespace sockets {

class UdpSocket {
public:
  UdpSocket() = default;
  static UdpSocket connect(const SocketAddr &addr, std::error_code &ec);

  void set_read_timeout(size_t timeout, std::error_code &ec);

  void set_write_timeout(size_t timeout, std::error_code &ec);

  size_t read_timeout(std::error_code &ec) const;

  size_t write_timeout(std::error_code &ec) const;

  // return receive size
  std::pair<size_t, SocketAddr> recv_from(char *buf, size_t buf_size,
                                          std::error_code &ec);

  // return send size
  size_t send_to(const char *buf, size_t buf_size, std::error_code &ec);
}; // class UdpSocket

} // namespace sockets

#endif // SOCKETS_UDPSOCKET_H
