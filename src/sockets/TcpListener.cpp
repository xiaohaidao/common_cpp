// Copyright (C) 2022 All rights reserved.
// Email: oxox0@qq.com. Created in 202207

#include "sockets/TcpListener.h"

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <sys/socket.h>
#include <unistd.h>
#endif // _WIN32

#include "utils/error_code.h"

#ifdef _WIN32
#define CLOSESOCKET closesocket
#else
#define CLOSESOCKET close
#define INVALID_SOCKET (socket_type)(~0)
#endif // _WIN32

TcpListener::TcpListener() : socket_(INVALID_SOCKET) {}

TcpListener::TcpListener(const socket_type &s) : socket_(s) {}

TcpListener TcpListener::bind(const char *port_or_service,
                              std::error_code &ec) {

  return bind(port_or_service, kIpV4, ec);
}

TcpListener TcpListener::bind(const char *port_or_service, FamilyType family,
                              std::error_code &ec) {

  TcpListener re;
  socket_type const listen = bind_port(port_or_service, family, ec).native();
  re.socket_ = listen;
  if (ec) {
    return re;
  }
  if (::listen(listen, SOMAXCONN)) {
    ::CLOSESOCKET(listen);
    ec = get_net_error_code();
    return re;
  }
  return re;
}

TcpStream TcpListener::bind_port(const char *port_or_service,
                                 std::error_code &ec) {

  return bind_port(port_or_service, kIpV4, ec);
}

TcpStream TcpListener::bind_port(const char *port_or_service, FamilyType family,
                                 std::error_code &ec) {

  TcpStream re;
  socket_type const listen = sockets::socket(family, kStream,
#ifdef __linux__
                                             family == kUnix ? kIp :
#endif // __linux__
                                                             kTCP,
                                             ec);
  if (listen == INVALID_SOCKET || ec) {
    return re;
  }
  re.socket_ = listen;
  sockets::set_reuseaddr(listen, ec);
  if (ec) {
    return re;
  }
  // Setup the TCP listening socket
  SocketAddr const addr =
#ifdef __linux__
      family == kUnix
          ? SocketAddr(port_or_service)
          :
#endif // __linux__
          SocketAddr::resolve_host(nullptr, port_or_service, ec, family, true);

  if (ec) {
    ::CLOSESOCKET(listen);
    return re;
  }
  if (::bind(listen, (sockaddr *)addr.native_addr(),
             static_cast<int>(addr.native_addr_size()))) {
    ::CLOSESOCKET(listen);
    ec = get_net_error_code();
    return re;
  }
  return re;
}

std::pair<TcpStream, SocketAddr> TcpListener::accept(std::error_code &ec) {
  std::pair<TcpStream, SocketAddr> re;

  // Accept a client socket
  socket_type const client = ::accept(socket_, NULL, NULL);
  if (client == INVALID_SOCKET) {
    ::CLOSESOCKET(client);
    ec = get_net_error_code();
    return re;
  }
  re.first.socket_ = client;
  re.second = SocketAddr::get_remote_socket(client, ec);
  return re;
}

void TcpListener::set_read_timeout(size_t timeout_ms, std::error_code &ec) {
  sockets::set_read_timeout(socket_, timeout_ms, ec);
}

size_t TcpListener::read_timeout(std::error_code &ec) const {
  return sockets::read_timeout(socket_, ec);
}

void TcpListener::close(std::error_code &ec) {
#ifdef __linux__
  auto addr = SocketAddr::get_local_socket(socket_, ec);
  if (addr.get_family() == kUnix) {
    ::unlink(addr.get_ip());
  }
#endif // __linux__
  if (::CLOSESOCKET(socket_)) {
    ec = get_net_error_code();
  }
}

socket_type TcpListener::native() const { return socket_; }
