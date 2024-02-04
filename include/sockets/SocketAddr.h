// Copyright (C) 2022 All rights reserved.
// Email: oxox0@qq.com. Created in 202207

#ifndef SOCKETS_SOCKETADDR_H
#define SOCKETS_SOCKETADDR_H

#include <cstdint>
#include <system_error>
#include <tuple>
#include <vector>

enum FamilyType {
  kUnspecified,
  kIpV4,
  kIpV6,
#ifdef __linux__
  kUnix
#endif // __linux__
};

enum SocketType {
  kStream,
  kDgram,
  kRaw,
};

enum Protocal {
  kIp,
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

void set_keepalive(socket_type s, std::error_code &ec, int enable,
                   int time_s = 7200, int intvl = 75, int times = 9);
void set_reuseaddr(socket_type s, std::error_code &ec);
void set_read_timeout(socket_type s, size_t timeout_ms, std::error_code &ec);
void set_write_timeout(socket_type s, size_t timeout_ms, std::error_code &ec);
size_t read_timeout(socket_type s, std::error_code &ec);
size_t write_timeout(socket_type s, std::error_code &ec);
int get_error_status(socket_type s, std::error_code &ec);

int enum_to_native(FamilyType family);
int enum_to_native(SocketType type);
int enum_to_native(Protocal protocal);

FamilyType native_to_family(int family);
SocketType native_to_type(int type);
Protocal native_to_protocal(int protocal);

uint16_t net_to_host(uint16_t v);
uint32_t net_to_host(uint32_t v);
uint64_t net_to_host(uint64_t v);
uint16_t host_to_net(uint16_t v);
uint32_t host_to_net(uint32_t v);
uint64_t host_to_net(uint64_t v);

} // namespace sockets

class SocketAddr {
public:
  SocketAddr();
  SocketAddr(const char *host_or_ip, const char *port_or_service,
             FamilyType family = kIpV4);
#ifdef __linux__
  SocketAddr(const char *path);
#endif // __linux__

  static SocketAddr get_local_socket(socket_type handle, std::error_code &ec);
  static SocketAddr get_remote_socket(socket_type handle, std::error_code &ec);

  static const char *get_localhost(std::error_code &ec);

  // @return [ip, mask, broadaddr]
  static std::vector<std::tuple<SocketAddr, SocketAddr, SocketAddr> >
  get_local_ip_mask(std::error_code &ec, FamilyType family = kIpV4);

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
  void set_port(unsigned short port);

  FamilyType get_family() const;
  int native_family() const;

  void *native_addr() const;
  size_t native_addr_size() const;
  void *native_ip_addr() const;

  // private:
  void get_nameinfo(char *host, size_t host_size, char *service,
                    size_t service_size, std::error_code &ec) const;

private:
  char ip_addr_[64];   // v4 16, v6 46
  char sock_addr_[64]; // struct sockaddr
};

#endif // SOCKETS_SOCKETADDR_H
