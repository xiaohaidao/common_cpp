// Copyright (C) 2022 All rights reserved.
// Email: oxox0@qq.com. Created in 202207

#ifndef SOCKETS_SOCKETADDR_H
#define SOCKETS_SOCKETADDR_H

#include <string>
#include <system_error>
#include <vector>

enum FamilyType {
  kUnspecified,
  kIpV4,
  kIpV6,
};

enum SocketType {
  kStream,
  kDgram,
  kRaw,
};

enum Protocal {
  kTCP,
  kUDP,
  kIcmp,
  kIcmpV6,
};

#ifdef _WIN32
typedef uint64_t socket_type;
#else  // _WIN32
typedef int socket_type;
#endif // _WIN32

namespace sockets {

socket_type socket(FamilyType family, SocketType type, Protocal protocal,
                   std::error_code &ec);

void setReuseAddr(socket_type s, std::error_code &ec);
void setReadTimeout(socket_type s, size_t timeout_ms, std::error_code &ec);
void setWriteTimeout(socket_type s, size_t timeout_ms, std::error_code &ec);
size_t readTimeout(socket_type s, std::error_code &ec);
size_t writeTimeout(socket_type s, std::error_code &ec);

int enumToNative(FamilyType family);
int enumToNative(SocketType type);
int enumToNative(Protocal protocal);

FamilyType nativeToFamily(int family);
SocketType nativeToType(int type);
Protocal nativeToProtocal(int protocal);

uint16_t netToHost(uint16_t v);
uint32_t netToHost(uint32_t v);
uint64_t netToHost(uint64_t v);
uint16_t hostToNet(uint16_t v);
uint32_t hostToNet(uint32_t v);
uint64_t hostToNet(uint64_t v);

} // namespace sockets

class SocketAddr {
public:
  SocketAddr();
  SocketAddr(const char *host_or_ip, const char *port_or_service);

  static SocketAddr get_local_socket(socket_type handle, std::error_code &ec);
  static SocketAddr get_remote_socket(socket_type handle, std::error_code &ec);

  static const char *get_localhost(std::error_code &ec);

  static SocketAddr resolve_host(const char *host, const char *port_or_service,
                                 std::error_code &ec, FamilyType family = kIpV4,
                                 bool bind = false);
  static std::vector<SocketAddr> resolve_host_all(const char *host,
                                                  const char *port_or_service,
                                                  std::error_code &ec,
                                                  FamilyType family = kIpV4);

  const char *get_ip() const;
  void get_ip(char *ip, size_t size, std::error_code &ec) const;

  unsigned short get_port() const;
  unsigned short native_port() const;

  FamilyType get_family() const;
  int native_family() const;

  void *native_addr() const;
  size_t native_addr_size() const;

  // private:
  void get_nameinfo(char *host, size_t host_size, char *service,
                    size_t service_size, std::error_code &ec) const;

private:
  char ip_addr_[64];   // v4 16, v6 46
  char sock_addr_[64]; // struct sockaddr
};

#endif // SOCKETS_SOCKETADDR_H
