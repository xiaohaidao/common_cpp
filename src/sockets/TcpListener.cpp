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

tcp_listener::tcp_listener() : socket_(INVALID_SOCKET) {}

tcp_listener::tcp_listener(const socket_type &s) : socket_(s) {}

tcp_listener tcp_listener::bind(const char *port_or_service,
                                std::error_code &ec) {

  return bind(port_or_service, kIpV4, ec);
}

tcp_listener tcp_listener::bind(const char *port_or_service, FamilyType family,
                                std::error_code &ec) {

  tcp_listener re;
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

tcp_stream tcp_listener::bind_port(const char *port_or_service,
                                   std::error_code &ec) {

  return bind_port(port_or_service, kIpV4, ec);
}

tcp_stream tcp_listener::bind_port(const char *port_or_service,
                                   FamilyType family, std::error_code &ec) {

  tcp_stream re;
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
  socket_addr const addr =
#ifdef __linux__
      family == kUnix
          ? socket_addr(port_or_service)
          :
#endif // __linux__
          socket_addr::resolve_host(nullptr, port_or_service, ec, family, true);

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

std::pair<tcp_stream, socket_addr> tcp_listener::accept(std::error_code &ec) {
  std::pair<tcp_stream, socket_addr> re;

  // Accept a client socket
  socket_type const client = ::accept(socket_, nullptr, nullptr);
  if (client == INVALID_SOCKET) {
    ::CLOSESOCKET(client);
    ec = get_net_error_code();
    return re;
  }
  re.first.socket_ = client;
  re.second = socket_addr::get_remote_socket(client, ec);
  return re;
}

void tcp_listener::set_read_timeout(size_t timeout_ms, std::error_code &ec) {
  sockets::set_read_timeout(socket_, ec, timeout_ms);
}

size_t tcp_listener::read_timeout(std::error_code &ec) const {
  return sockets::read_timeout(socket_, ec);
}

void tcp_listener::close(std::error_code &ec) {
#ifdef __linux__
  auto addr = socket_addr::get_local_socket(socket_, ec);
  if (addr.get_family() == kUnix) {
    ::unlink(addr.get_ip());
  }
#endif // __linux__
  if (::CLOSESOCKET(socket_)) {
    ec = get_net_error_code();
  }
}

socket_type tcp_listener::native() const { return socket_; }
