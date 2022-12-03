// Copyright (C) 2022 All rights reserved.
// Email: oxox0@qq.com. Created in 202207

#ifdef _WIN32

#include "sockets/TcpStream.h"

#include <winsock2.h>
#include <ws2tcpip.h>

#include "utils/error_code.h"
#include "utils/macro.h"

namespace sockets {

TcpStream::TcpStream() : socket_(INVALID_SOCKET) {}

TcpStream::TcpStream(const socket_type &s) : socket_(s) {}

TcpStream TcpStream::connect(const SocketAddr &addr, std::error_code &ec) {
  TcpStream re;
  SOCKET connect = socket(addr.get_family(), kStream, kTCP, ec);
  if (ec) {
    return re;
  }
  re.socket_ = connect;
  // // If mode = 0, blocking is enabled;
  // // If mode != 0, non-blocking mode is enabled.
  // u_long mode = 1;
  // if (ioctlsocket(connect, FIONBIO, &mode)) {
  //   ec = getNetErrorCode();
  //   return re;
  // }

  if (::connect(connect, (const sockaddr *)addr.native_addr(),
                (int)addr.native_addr_size())) {
    ec = getNetErrorCode();
    ::closesocket(connect);
  }
  return re;
}

void TcpStream::set_read_timeout(size_t timeout, std::error_code &ec) {
  DWORD time_out = timeout;
  if (!::setsockopt(socket_, SOL_SOCKET, SO_RCVTIMEO, (const char *)&time_out,
                    sizeof(time_out))) {
    ec = getNetErrorCode();
    return;
  }
  read_timeout_ = timeout;
}

void TcpStream::set_write_timeout(size_t timeout, std::error_code &ec) {
  DWORD time_out = timeout;
  if (!::setsockopt(socket_, SOL_SOCKET, SO_SNDTIMEO, (const char *)&time_out,
                    sizeof(time_out))) {
    ec = getNetErrorCode();
    return;
  }
  send_timeout_ = timeout;
}

size_t TcpStream::read_timeout(std::error_code &ec) const {
  // int v = 0, s = sizeof(v);
  // if (!::getsockopt(socket_, SOL_SOCKET, SO_RCVTIMEO, (char *)&v, &s)) {
  //   printf("recive buff size : %d", v);
  // }
  return read_timeout_;
}

size_t TcpStream::write_timeout(std::error_code &ec) const {
  return send_timeout_;
}

size_t TcpStream::read(char *buf, size_t buf_size, std::error_code &ec) {
  DWORD re_size = 0, flag = 0;
  WSABUF data{(ULONG)buf_size, buf};
  if (!::WSARecv(socket_, &data, 1, &re_size, &flag, nullptr, nullptr)) {
    ec = getNetErrorCode();
  }
  return re_size;
}

size_t TcpStream::write(const char *buf, size_t buf_size, std::error_code &ec) {
  DWORD re_size = 0;
  WSABUF data{(ULONG)buf_size, (char *)buf};
  if (!::WSASend(socket_, &data, 1, &re_size, 0, nullptr, nullptr)) {
    ec = getNetErrorCode();
  }
  return re_size;
}

void TcpStream::close(std::error_code &ec) {
  if (::shutdown(socket_, SD_BOTH)) {
    ec = getNetErrorCode();
  }
  if (::closesocket(socket_)) {
    ec = getNetErrorCode();
  }
}

socket_type TcpStream::native_handle() const { return socket_; }

} // namespace sockets

#endif // _WIN32
