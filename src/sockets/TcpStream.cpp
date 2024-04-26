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
#define CLOSESOCKET closesocket
#else
#define CLOSESOCKET close
#define INVALID_SOCKET (socket_type)(~0)
#define SD_SEND (SHUT_WR)
#define SD_BOTH (SHUT_RDWR)
#endif // _WIN32

TcpStream::TcpStream() : socket_(INVALID_SOCKET) {}

TcpStream::TcpStream(const socket_type &s) : socket_(s) {}

TcpStream TcpStream::connect(const SocketAddr &addr, std::error_code &ec) {
  TcpStream re;
  socket_type const connect = sockets::socket(addr.get_family(), kStream,
#ifdef __linux__
                                              addr.get_family() == kUnix ? kIp :
#endif // __linux__
                                                                         kTCP,
                                              ec);

  if (ec) {
    return re;
  }
  re.socket_ = connect;
  // // windows platform
  // // If mode = 0, blocking is enabled;
  // // If mode != 0, non-blocking mode is enabled.
  // u_long mode = 1;
  // if (ioctlsocket(connect, FIONBIO, &mode)) {
  //   ec = get_net_error_code();
  //   return re;
  // }

  re.connected(addr, ec);
  return re;
}

void TcpStream::connected(const SocketAddr &addr, std::error_code &ec) {
  if (::connect(socket_, (const sockaddr *)addr.native_addr(),
                (int)addr.native_addr_size())) {
    ec = get_net_error_code();
    ::CLOSESOCKET(socket_);
  }
}

void TcpStream::set_read_timeout(size_t timeout_ms, std::error_code &ec) {
  sockets::set_read_timeout(socket_, ec, timeout_ms);
}

void TcpStream::set_write_timeout(size_t timeout_ms, std::error_code &ec) {
  sockets::set_write_timeout(socket_, ec, timeout_ms);
}

size_t TcpStream::read_timeout(std::error_code &ec) const {
  return sockets::read_timeout(socket_, ec);
}

size_t TcpStream::write_timeout(std::error_code &ec) const {
  return sockets::write_timeout(socket_, ec);
}

int TcpStream::read(char *buff, size_t buff_size, std::error_code &ec) {
  int const re_size = ::recv(socket_, buff, static_cast<int>(buff_size), 0);
  if (re_size < 0) {
    ec = get_net_error_code();
  }
  return re_size;
}

int TcpStream::write(const char *buff, size_t buff_size, std::error_code &ec) {
#ifdef _WIN32
#define MSG_NOSIGNAL 0
#endif
  int const re_size =
      ::send(socket_, buff, static_cast<int>(buff_size), MSG_NOSIGNAL);
  if (re_size < 0) {
    ec = get_net_error_code();
  }
  return re_size;
}

void TcpStream::shutdown(std::error_code &ec) {
  if (::shutdown(socket_, SD_SEND)) {
    std::error_code const re_ec = get_net_error_code();
    if (ENOTCONN != re_ec.value()) {
      ec = re_ec;
    }
  }
}

void TcpStream::close(std::error_code &ec) {
  shutdown(ec);
  if (::CLOSESOCKET(socket_)) {
    ec = get_net_error_code();
  }
}

socket_type TcpStream::native() const { return socket_; }
