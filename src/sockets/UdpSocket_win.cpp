// Copyright (C) 2022 All rights reserved.
// Email: oxox0@qq.com. Created in 202207

#ifdef _WIN32

#include "sockets/UdpSocket.h"

#include <winsock2.h>
#include <ws2tcpip.h>

#include "utils/error_code.h"

namespace sockets {

UdpSocket UdpSocket::connect(const SocketAddr &addr, std::error_code &ec) {}

UdpSocket UdpSocket::bind(const SocketAddr &addr, std::error_code &ec) {}

void UdpSocket::set_read_timeout(size_t timeout, std::error_code &ec) {
  DWORD time_out = timeout;
  if (!::setsockopt(socket_, SOL_SOCKET, SO_RCVTIMEO, (const char *)&time_out,
                    sizeof(time_out))) {
    ec = getNetErrorCode();
    return;
  }
  read_timeout_ = timeout;
}

void UdpSocket::set_write_timeout(size_t timeout, std::error_code &ec) {
  DWORD time_out = timeout;
  if (!::setsockopt(socket_, SOL_SOCKET, SO_SNDTIMEO, (const char *)&time_out,
                    sizeof(time_out))) {
    ec = getNetErrorCode();
    return;
  }
  send_timeout_ = timeout;
}

size_t UdpSocket::read_timeout(std::error_code &ec) const {
  unsigned int v = 0, s = sizeof(v);
  if (!::getsockopt(socket_, SOL_SOCKET, SO_MAX_MSG_SIZE, (char *)&v,
                    (int *)&s)) {
    printf("unsigned int buff size : %d\n", v);
  }
  return read_timeout_;
}

size_t UdpSocket::write_timeout(std::error_code &ec) const {
  return send_timeout_;
}

std::pair<size_t, SocketAddr> UdpSocket::recv_from(char *buf, size_t buf_size,
                                                   std::error_code &ec) {}

size_t UdpSocket::send_to(const char *buf, size_t buf_size,
                          std::error_code &ec) {}

} // namespace sockets

#endif // _WIN32
