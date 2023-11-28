// Copyright (C) 2022 All rights reserved.
// Email: oxox0@qq.com. Created in 202207

#include "sockets/SocketAddr.h"

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#endif // _WIN32

#include "utils/error_code.h"

namespace sockets {

namespace {

#ifdef _WIN32

#if __BIG_ENDIAN__
#define htonll(x) (x)
#define ntohll(x) (x)
#else
#define htonll(x) ((((uint64_t)htonl(x & 0xFFFFFFFF)) << 32) + htonl(x >> 32))
#define ntohll(x) ((((uint64_t)ntohl(x & 0xFFFFFFFF)) << 32) + ntohl(x >> 32))
#endif

#endif // _WIN32

} // namespace

#ifndef _WIN32
#define INVALID_SOCKET -1
#endif // _WIN32

socket_type socket(FamilyType family, SocketType type, Protocal protocal,
                   std::error_code &ec) {
  socket_type s = ::socket(enum_to_native(family), enum_to_native(type),
                           enum_to_native(protocal));
  if (s == INVALID_SOCKET) {
    ec = get_net_error_code();
  }
  return s;
}

void set_keepalive(socket_type s, std::error_code &ec, int enable, int time_s,
                   int intvl, int times) {

  /*
  #ifdef _WIN32
    struct tcp_keepalive {
      u_long onoff;
      u_long keepalivetime;
      u_long keepaliveinterval;
    } keepalive;

    // times is 10, can't be changed
    // #define SIO_KEEPALIVE_VALS _WSAIOW(IOC_VENDOR, 4)

    keepalive.onoff = enable;
    keepalive.keepalivetime = time_s * 1000;
    keepalive.keepaliveinterval = intvl * 1000;
    DWORD ret;
    if (::WSAIoctl(s,                  // descriptor identifying a
                   SIO_KEEPALIVE_VALS, // dwIoControlCode
                   &keepalive,         // pointer to tcp_keepalive
                   sizeof(keepalive),  // length of input buffer
                   NULL,               // output buffer
                   0,                  // size of output buffer
                   &ret,               // number of bytes returned
                   nullptr,            // OVERLAPPED structure
                   nullptr             // completion routine
                   ) == SOCKET_ERROR) {

      ec = get_net_error_code();
    }

  #else  // Windows 10, version 1709. support
  #endif // _WIN32
  */

  if (::setsockopt(s, SOL_SOCKET, SO_KEEPALIVE, (const char *)&enable,
                   sizeof(enable)) < 0) {
    ec = get_net_error_code();
    return;
  }
  if (::setsockopt(s, IPPROTO_TCP, TCP_KEEPIDLE, (const char *)&time_s,
                   sizeof(time_s)) < 0) {
    ec = get_net_error_code();
    return;
  }
  if (::setsockopt(s, IPPROTO_TCP, TCP_KEEPINTVL, (const char *)&intvl,
                   sizeof(intvl)) < 0) {
    ec = get_net_error_code();
    return;
  }
  if (::setsockopt(s, IPPROTO_TCP, TCP_KEEPCNT, (const char *)&times,
                   sizeof(times)) < 0) {
    ec = get_net_error_code();
    return;
  }
}

void set_reuseaddr(socket_type s, std::error_code &ec) {
  constexpr int set = 1;
#ifdef SO_REUSEPORT
  int optname = SO_REUSEPORT;
#else
  int optname = SO_REUSEADDR;
#endif // SO_REUSEPORT
  if (::setsockopt(s, SOL_SOCKET, optname, (const char *)&set, sizeof(set)) <
      0) {
    ec = get_net_error_code();
  }
}

void set_read_timeout(socket_type s, size_t timeout_ms, std::error_code &ec) {
#ifdef _WIN32
  size_t time_out = timeout_ms;
#else
  struct timeval time_out = {};
  time_out.tv_usec = timeout_ms % 1000 * 1000;
  time_out.tv_sec = timeout_ms / 1000;
#endif // _WIN32
  if (::setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, (const char *)&time_out,
                   sizeof(time_out)) < 0) {
    ec = get_net_error_code();
    return;
  }
}

void set_write_timeout(socket_type s, size_t timeout_ms, std::error_code &ec) {
#ifdef _WIN32
  size_t time_out = timeout_ms;
#else
  struct timeval time_out = {};
  time_out.tv_usec = timeout_ms % 1000 * 1000;
  time_out.tv_sec = timeout_ms / 1000;
#endif // _WIN32
  if (::setsockopt(s, SOL_SOCKET, SO_SNDTIMEO, (const char *)&time_out,
                   sizeof(time_out)) < 0) {
    ec = get_net_error_code();
  }
}

size_t read_timeout(socket_type s, std::error_code &ec) {
#ifdef _WIN32
  size_t v = 0;
  socklen_t size = sizeof(v);
  if (::getsockopt(s, SOL_SOCKET, SO_RCVTIMEO, (char *)&v, &size) < 0) {
    ec = get_net_error_code();
  }
  return v;
#else
  struct timeval v = {};
  socklen_t size = sizeof(v);
  if (::getsockopt(s, SOL_SOCKET, SO_RCVTIMEO, (void *)&v, &size) < 0) {
    ec = get_net_error_code();
  }
  return v.tv_sec * 1000 + v.tv_usec / 1000;
#endif // _WIN32
}

size_t write_timeout(socket_type s, std::error_code &ec) {
#ifdef _WIN32
  size_t v = 0;
  socklen_t size = sizeof(v);
  if (::getsockopt(s, SOL_SOCKET, SO_SNDTIMEO, (char *)&v, &size) < 0) {
    ec = get_net_error_code();
  }
  return v;
#else
  struct timeval v = {};
  socklen_t size = sizeof(v);
  if (::getsockopt(s, SOL_SOCKET, SO_SNDTIMEO, (void *)&v, &size) < 0) {
    ec = get_net_error_code();
  }
  return v.tv_sec * 1000 + v.tv_usec / 1000;
#endif // _WIN32
}

int get_error_status(socket_type s, std::error_code &ec) {
  int set = 0;
  socklen_t ret = sizeof(set);
  if (::getsockopt(s, SOL_SOCKET, SO_ERROR, (char *)&set, &ret) < 0) {
    ec = get_net_error_code();
    return -1;
  }
  if (set != 0) {
    ec = {set, std::system_category()};
  }
  return set;
}

int enum_to_native(FamilyType family) {
  switch (family) {
  case kIpV4:
    return AF_INET;
  case kIpV6:
    return AF_INET6;
  case kUnspecified:
  default:
    return AF_UNSPEC;
  }
}

int enum_to_native(SocketType type) {
  switch (type) {
  case kDgram:
    return SOCK_DGRAM;
  case kRaw:
    return SOCK_RAW;
  case kStream:
  default:
    return SOCK_STREAM;
  }
}

int enum_to_native(Protocal protocal) {
  switch (protocal) {
  case kIp:
    return IPPROTO_IP;
  case kUDP:
    return IPPROTO_UDP;
  case kIcmp:
    return IPPROTO_ICMP;
  case kIcmpV6:
    return IPPROTO_ICMPV6;
  case kTCP:
  default:
    return IPPROTO_TCP;
  }
}

FamilyType native_to_family(int family) {
  switch (family) {
  case AF_INET:
    return kIpV4;
  case AF_INET6:
    return kIpV6;
  case AF_UNSPEC:
  default:
    return kUnspecified;
  }
}

SocketType native_to_type(int type) {
  switch (type) {
  case SOCK_DGRAM:
    return kDgram;
  case SOCK_RAW:
    return kRaw;
  case SOCK_STREAM:
  default:
    return kStream;
  }
}

Protocal native_to_protocal(int protocal) {
  switch (protocal) {
  case IPPROTO_UDP:
    return kUDP;
  case IPPROTO_ICMP:
    return kIcmp;
  case IPPROTO_ICMPV6:
    return kIcmpV6;
  case IPPROTO_TCP:
  default:
    return kTCP;
  }
}

uint16_t net_to_host(uint16_t v) { return ntohs(v); }

uint32_t net_to_host(uint32_t v) { return ntohl(v); }

uint64_t net_to_host(uint64_t v) {
  return
#ifdef _WIN32
      ntohll(v);
#else
      be64toh(v);
#endif // _WIN32
}

uint16_t host_to_net(uint16_t v) { return htons(v); }

uint32_t host_to_net(uint32_t v) { return htonl(v); }

uint64_t host_to_net(uint64_t v) {
  return
#ifdef _WIN32
      htonll(v);
#else
      htobe64(v);
#endif // _WIN32
}

} // namespace sockets
