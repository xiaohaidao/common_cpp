// Copyright (C) 2022 All rights reserved.
// Email: oxox0@qq.com. Created in 202207

#include "sockets/SocketAddr.h"

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#endif // _WIN32

#include "utils/error_code.h"

namespace sockets {

namespace {

#ifdef _WIN32
class SocketStart {
public:
  SocketStart() {
    std::error_code ec;
    initSocket(ec);
  }

  ~SocketStart() {
    std::error_code ec;
    uninitSocket(ec);
  }

private:
  void initSocket(std::error_code &ec) {
    WSADATA wsa_data;
    if (WSAStartup(MAKEWORD(2, 2), &wsa_data)) {
      ec = getNetErrorCode();
    }
  }
  void uninitSocket(std::error_code &ec) {
    if (WSACleanup()) {
      ec = getNetErrorCode();
    }
  }

} g_socket_start;

#endif // _WIN32

} // namespace

#ifdef _WIN32
typedef int size_type;
#else
typedef socklen_t size_type;
#endif // _WIN32

} // namespace sockets

using namespace sockets;

// inet_ntoa(sin_addr); // ipv4
// inet_ntop(sin_addr);  ipaddr to str ip
// inet_pton(ip);
// ntohs(sin_port); // port
// htons(port); // to sin_port
// inet_addr(ip); // str ip to sin_ipaddr
SocketAddr::SocketAddr() : ip_addr_{}, sock_addr_{} {}

SocketAddr::SocketAddr(const char *host_or_ip, const char *port_or_service)
    : ip_addr_{}, sock_addr_{} {
  std::error_code ec;
  *this = resolve_host(host_or_ip, port_or_service, ec);
}

const char *SocketAddr::get_ip() const {
  if (ip_addr_[0] == 0) {
    std::error_code ec;
    get_ip((char *)ip_addr_, sizeof(ip_addr_), ec);
  }
  return ip_addr_;
}

void SocketAddr::get_ip(char *ip, size_t size, std::error_code &ec) const {
  FamilyType family = get_family();
  void *sin_addr = nullptr;
  if (family == kIpV4) {
    sin_addr = &((struct sockaddr_in *)native_addr())->sin_addr;
  } else {
    sin_addr = &((struct sockaddr_in6 *)native_addr())->sin6_addr;
  }
  if (!::inet_ntop(enumToNative(family), sin_addr, ip, size)) {
    ec = getNetErrorCode();
  }
}

unsigned short SocketAddr::get_port() const { return ::ntohs(native_port()); }

unsigned short SocketAddr::native_port() const {
  // ((struct sockaddr_in *)native_addr())->sin_port = ::htons(port);
  return ((struct sockaddr_in *)native_addr())->sin_port;
}

FamilyType SocketAddr::get_family() const {
  return nativeToFamily(native_family());
}

int SocketAddr::native_family() const {
  return ((struct sockaddr_in *)native_addr())->sin_family;
}

void *SocketAddr::native_addr() const { return (void *)sock_addr_; }

size_t SocketAddr::native_addr_size() const {
  return get_family() == kIpV4 ? sizeof(struct sockaddr_in)
                               : sizeof(struct sockaddr_in6);
}

SocketAddr SocketAddr::get_local_socket(socket_type s, std::error_code &ec) {
  SocketAddr re;
  sockaddr *addr = (sockaddr *)re.sock_addr_;
  size_type size = static_cast<size_type>(re.native_addr_size());
  if (::getsockname(s, addr, &size)) {
    ec = getNetErrorCode();
    return re;
  }
  re.get_ip(re.ip_addr_, sizeof(re.ip_addr_), ec);
  return re;
}

SocketAddr SocketAddr::get_remote_socket(socket_type s, std::error_code &ec) {
  SocketAddr re;
  sockaddr *addr = (sockaddr *)re.sock_addr_;
  size_type size = static_cast<size_type>(re.native_addr_size());
  if (::getpeername(s, addr, &size)) {
    ec = getNetErrorCode();
    return re;
  }
  re.get_ip(re.ip_addr_, sizeof(re.ip_addr_), ec);
  return re;
}

void SocketAddr::get_nameinfo(char *host, size_t host_size, char *service,
                              size_t service_size, std::error_code &ec) const {

  if (::getnameinfo((const sockaddr *)native_addr(),
                    static_cast<size_type>(native_addr_size()), host,
                    static_cast<size_type>(host_size), service,
                    static_cast<size_type>(service_size), NI_NUMERICSERV)) {

    ec = getNetErrorCode();
  }
}

const char *SocketAddr::get_localhost(std::error_code &ec) {
  static char buff[32] = {};
  if (buff[0] == 0 && ::gethostname(buff, sizeof(buff))) {
    ec = getNetErrorCode();
    return nullptr;
  }
  return buff;
}

SocketAddr SocketAddr::resolve_host(const char *host,
                                    const char *port_or_service,
                                    std::error_code &ec, FamilyType family,
                                    bool bind) {
  SocketAddr re;
  const char *host_name = host;

  struct addrinfo hints;
  struct addrinfo *result = nullptr;
  memset(&hints, 0, sizeof(hints));
  hints.ai_family = enumToNative(family);
  // hints.ai_family = AF_UNSPEC;
  // hints.ai_flags = AI_NUMERICHOST;
  // hints.ai_socktype = SOCK_STREAM;
  // hints.ai_protocol = IPPROTO_TCP;
  if (bind) {
    hints.ai_flags = AI_PASSIVE;
  }

  if (::getaddrinfo(host_name, port_or_service, &hints, &result)) {
    ec = getNetErrorCode();
    return re;
  }

  for (auto ptr = result; ptr != NULL; ptr = ptr->ai_next) {
    memcpy(re.sock_addr_, ptr->ai_addr, re.native_addr_size());
    // sockaddr_ipv4->ai_canonname
    re.get_ip(re.ip_addr_, sizeof(re.ip_addr_), ec);
    break;
  }
  ::freeaddrinfo(result);
  return re;
}

std::vector<SocketAddr>
SocketAddr::resolve_host_all(const char *host, const char *port_or_service,
                             std::error_code &ec, FamilyType family) {

  std::vector<SocketAddr> re;
  const char *host_name = host;

  struct addrinfo hints;
  struct addrinfo *result = nullptr;
  memset(&hints, 0, sizeof(hints));
  hints.ai_family = enumToNative(family);

  if (::getaddrinfo(host_name, port_or_service, &hints, &result)) {
    ec = getNetErrorCode();
    return re;
  }

  for (auto ptr = result; ptr != NULL; ptr = ptr->ai_next) {
    SocketAddr address;
    memcpy(address.sock_addr_, ptr->ai_addr,
           ptr->ai_family == AF_INET ? sizeof(sockaddr_in)
                                     : sizeof(sockaddr_in6));
    address.get_ip(address.ip_addr_, sizeof(address.ip_addr_), ec);
    re.push_back(std::move(address));
  }

  ::freeaddrinfo(result);
  return re;
}
