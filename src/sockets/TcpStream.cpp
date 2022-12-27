// Copyright (C) 2022 All rights reserved.
// Email: oxox0@qq.com. Created in 202207

#include "sockets/TcpStream.h"

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <sys/socket.h>
#include <unistd.h>
#endif // _WIN32

#include "utils/error_code.h"
#include "utils/macro.h"

#ifdef _WIN32
#define closesocket closesocket
#else
#define closesocket close
#define INVALID_SOCKET (socket_type)(~0)
#define SD_BOTH (SHUT_RDWR)
#endif // _WIN32

namespace sockets {

TcpStream::TcpStream()
    : socket_(INVALID_SOCKET)
#ifdef _WIN32
      ,
      read_timeout_(-1), send_timeout_(-1)
#endif // _WIN32
{
}

TcpStream::TcpStream(const socket_type &s)
    : socket_(s)
#ifdef _WIN32
      ,
      read_timeout_(-1), send_timeout_(-1)
#endif // _WIN32
{
}

TcpStream TcpStream::connect(const SocketAddr &addr, std::error_code &ec) {
  TcpStream re;
  socket_type connect = socket(addr.get_family(), kStream, kTCP, ec);
  if (ec) {
    return re;
  }
  re.socket_ = connect;
  // // windows platform
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
  size_t time_out = timeout;
  if (!::setsockopt(socket_, SOL_SOCKET, SO_RCVTIMEO, (const char *)&time_out,
                    sizeof(time_out))) {
    ec = getNetErrorCode();
    return;
  }
#ifdef _WIN32
  read_timeout_ = timeout;
#endif // _WIN32
}

void TcpStream::set_write_timeout(size_t timeout, std::error_code &ec) {
  size_t time_out = timeout;
  if (!::setsockopt(socket_, SOL_SOCKET, SO_SNDTIMEO, (const char *)&time_out,
                    sizeof(time_out))) {
    ec = getNetErrorCode();
    return;
  }
#ifdef _WIN32
  send_timeout_ = timeout;
#endif // _WIN32
}

size_t TcpStream::read_timeout(std::error_code &ec) const {
#ifdef _WIN32
  return read_timeout_;
#else
  socklen_t v = 0, s = sizeof(v);
  if (!::getsockopt(socket_, SOL_SOCKET, SO_RCVTIMEO, (void *)&v, &s)) {
    ec = getNetErrorCode();
  }
  return v;
#endif // _WIN32
}

size_t TcpStream::write_timeout(std::error_code &ec) const {
#ifdef _WIN32
  return send_timeout_;
#else
  socklen_t v = 0, s = sizeof(v);
  if (!::getsockopt(socket_, SOL_SOCKET, SO_SNDTIMEO, (void *)&v, &s)) {
    ec = getNetErrorCode();
  }
  return v;
#endif // _WIN32
}

int TcpStream::read(char *buff, size_t buff_size, std::error_code &ec) {
  int re_size = ::recv(socket_, buff, buff_size, 0);
  if (re_size < 0) {
    ec = getNetErrorCode();
  }
  return re_size;
}

int TcpStream::write(const char *buff, size_t buff_size, std::error_code &ec) {
  int re_size = ::send(socket_, buff, buff_size, 0);
  if (re_size < 0) {
    ec = getNetErrorCode();
  }
  return re_size;
}

void TcpStream::close(std::error_code &ec) {
  if (::shutdown(socket_, SD_BOTH)) {
    std::error_code re_ec = getNetErrorCode();
    if (ENOTCONN != re_ec.value()) {
      ec = re_ec;
    }
  }
  if (::closesocket(socket_)) {
    ec = getNetErrorCode();
  }
}

socket_type TcpStream::native_handle() const { return socket_; }

} // namespace sockets
