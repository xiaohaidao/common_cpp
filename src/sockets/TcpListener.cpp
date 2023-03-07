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
#define closesocket closesocket
#else
#define closesocket close
#define INVALID_SOCKET (socket_type)(~0)
#endif // _WIN32

TcpListener::TcpListener()
    : socket_(INVALID_SOCKET)

#ifdef _WIN32
      ,
      read_timeout_(0), send_timeout_(0)
#endif // _WIN32
{
}

TcpListener::TcpListener(const socket_type &s)
    : socket_(s)
#ifdef _WIN32
      ,
      read_timeout_(0), send_timeout_(0)
#endif // _WIN32
{
}

TcpListener TcpListener::bind(const char *port_or_service,
                              std::error_code &ec) {

  return bind(port_or_service, kIpV4, ec);
}

TcpListener TcpListener::bind(const char *port_or_service, FamilyType family,
                              std::error_code &ec) {

  TcpListener re;
  socket_type listen = bind_port(port_or_service, family, ec).native_handle();
  re.socket_ = listen;
  if (ec) {
    return re;
  }
  if (::listen(listen, SOMAXCONN)) {
    ::closesocket(listen);
    ec = getNetErrorCode();
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
  socket_type listen = sockets::socket(family, kStream, kTCP, ec);
  if (listen == INVALID_SOCKET || ec) {
    return re;
  }
  re.socket_ = listen;
  sockets::setReuseAddr(listen, ec);
  if (ec) {
    return re;
  }
  // Setup the TCP listening socket
  SocketAddr addr =
      SocketAddr::resolve_host(nullptr, port_or_service, ec, family, true);
  if (ec) {
    ::closesocket(listen);
    return re;
  }
  if (::bind(listen, (sockaddr *)addr.native_addr(), addr.native_addr_size())) {
    ::closesocket(listen);
    ec = getNetErrorCode();
    return re;
  }
  return re;
}

std::pair<TcpStream, SocketAddr> TcpListener::accept(std::error_code &ec) {
  std::pair<TcpStream, SocketAddr> re;

  // Accept a client socket
  socket_type client = ::accept(socket_, NULL, NULL);
  if (client == INVALID_SOCKET) {
    ::closesocket(client);
    ec = getNetErrorCode();
    return re;
  }
  re.first.socket_ = client;
  re.second = SocketAddr::get_remote_socket(client, ec);
  return re;
}

void TcpListener::set_read_timeout(size_t timeout_ms, std::error_code &ec) {
#ifdef _WIN32
  read_timeout_ = timeout_ms;
#endif // _WIN32
  sockets::setReadTimeout(socket_, timeout_ms, ec);
}

size_t TcpListener::read_timeout(std::error_code &ec) const {
#ifdef _WIN32
  return read_timeout_;
#else
  return sockets::readTimeout(socket_, ec);
#endif // _WIN32
}

void TcpListener::close(std::error_code &ec) {
  if (::closesocket(socket_)) {
    ec = getNetErrorCode();
  }
}

socket_type TcpListener::native_handle() const { return socket_; }
