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

tcp_stream::tcp_stream() : socket_(INVALID_SOCKET) {}

tcp_stream::tcp_stream(const socket_type &s) : socket_(s) {}

tcp_stream tcp_stream::connect(const socket_addr &addr, std::error_code &ec) {
  tcp_stream re;
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

void tcp_stream::connected(const socket_addr &addr, std::error_code &ec) {
  if (::connect(socket_, (const sockaddr *)addr.native_addr(),
                (int)addr.native_addr_size())) {
    ec = get_net_error_code();
    ::CLOSESOCKET(socket_);
  }
}

void tcp_stream::set_read_timeout(size_t timeout_ms, std::error_code &ec) {
  sockets::set_read_timeout(socket_, ec, timeout_ms);
}

void tcp_stream::set_write_timeout(size_t timeout_ms, std::error_code &ec) {
  sockets::set_write_timeout(socket_, ec, timeout_ms);
}

size_t tcp_stream::read_timeout(std::error_code &ec) const {
  return sockets::read_timeout(socket_, ec);
}

size_t tcp_stream::write_timeout(std::error_code &ec) const {
  return sockets::write_timeout(socket_, ec);
}

int tcp_stream::read(char *buff, size_t buff_size, std::error_code &ec) {
  int const re_size = ::recv(socket_, buff, static_cast<int>(buff_size), 0);
  if (re_size < 0) {
    ec = get_net_error_code();
  }
  return re_size;
}

int tcp_stream::write(const char *buff, size_t buff_size, std::error_code &ec) {
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

void tcp_stream::shutdown(std::error_code &ec) {
  if (::shutdown(socket_, SD_SEND)) {
    std::error_code const re_ec = get_net_error_code();
    if (ENOTCONN != re_ec.value()) {
      ec = re_ec;
    }
  }
}

void tcp_stream::close(std::error_code &ec) {
  shutdown(ec);
  if (::CLOSESOCKET(socket_)) {
    ec = get_net_error_code();
  }
}

socket_type tcp_stream::native() const { return socket_; }
