// Copyright (C) 2022 All rights reserved.
// Email: oxox0@qq.com. Created in 202207

#ifdef _WIN32

#include "sockets/TcpListener.h"

#include <winsock2.h>
#include <ws2tcpip.h>

#include "utils/error_code.h"

namespace sockets {

TcpListener::TcpListener() : socket_(INVALID_SOCKET) {}

TcpListener::TcpListener(const socket_type &s) : socket_(s) {}

TcpListener TcpListener::bind(const char *port_or_service,
                              std::error_code &ec) {

  TcpListener re;
  FamilyType family = kIpV4;
  SOCKET listen = socket(family, kStream, kTCP, ec);
  if (listen == INVALID_SOCKET || ec) {
    return re;
  }
  re.socket_ = listen;
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
  if (::listen(listen, SOMAXCONN)) {
    ::closesocket(listen);
    ec = getNetErrorCode();
    return re;
  }
  return re;
}

std::pair<TcpStream, SocketAddr> TcpListener::accept(std::error_code &ec) {
  std::pair<TcpStream, SocketAddr> re;

  // Accept a client socket
  SOCKET client = ::accept(socket_, NULL, NULL);
  if (client == INVALID_SOCKET) {
    ::closesocket(client);
    ec = getNetErrorCode();
    return re;
  }
  // char addr[64 * 3] = {};
  // SOCKET client = socket(kUnspecified, kStream, kTCP, ec);
  // if (client == INVALID_SOCKET || ec) {
  //   return re;
  // }
  // DWORD bytes_received = 0;
  // printf("sockaddr size %d %d %d\n", sizeof(sockaddr), sizeof(sockaddr) + 16,
  //        sizeof(sockaddr_in6));
  // if (!::AcceptEx(socket_, client, addr, 64, 64, 64,
  //                 &bytes_received, nullptr)) {

  //   ::closesocket(client);
  //   ec = getNetErrorCode();
  //   return re;
  // }
  re.first.socket_ = client;
  re.second = SocketAddr::get_remote_socket(client, ec);
  // memcpy(re.second.native_addr(), addr + 128, 32);
  // for (size_t i = 0; i < 64 * 3; ++i) {
  //   printf("%x",addr[i]);
  // }
  // printf("\n");
  return re;
}

void TcpListener::close(std::error_code &ec) {
  if (::closesocket(socket_)) {
    ec = getNetErrorCode();
  }
}

socket_type TcpListener::native_handle() const { return socket_; }

} // namespace sockets

#endif // _WIN32
