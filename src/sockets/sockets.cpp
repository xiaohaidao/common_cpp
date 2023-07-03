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
  socket_type s = ::socket(enumToNative(family), enumToNative(type),
                           enumToNative(protocal));
  if (s == INVALID_SOCKET) {
    ec = getNetErrorCode();
  }
  return s;
}

void setReuseAddr(socket_type s, std::error_code &ec) {
  constexpr int set = 1;
#ifdef SO_REUSEPORT
  if (setsockopt(s, SOL_SOCKET, SO_REUSEPORT, (const char *)&set, sizeof(set)) <
      0) {
#else
  if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (const char *)&set, sizeof(set)) <
      0) {
#endif // SO_REUSEPORT
    ec = getNetErrorCode();
  }
}

void setReadTimeout(socket_type s, size_t timeout_ms, std::error_code &ec) {
#ifdef _WIN32
  size_t time_out = timeout_ms;
  if (::setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, (const char *)&time_out,
                   sizeof(time_out)) < 0) {
    ec = getNetErrorCode();
    return;
  }
#else
  struct timeval time_out = {};
  time_out.tv_usec = timeout_ms % 1000 * 1000;
  time_out.tv_sec = timeout_ms / 1000;
  if (::setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, (const char *)&time_out,
                   sizeof(time_out)) < 0) {
    ec = getNetErrorCode();
    return;
  }
#endif // _WIN32
}

void setWriteTimeout(socket_type s, size_t timeout_ms, std::error_code &ec) {
#ifdef _WIN32
  size_t time_out = timeout_ms;
  if (::setsockopt(s, SOL_SOCKET, SO_SNDTIMEO, (const char *)&time_out,
                   sizeof(time_out)) < 0) {
    ec = getNetErrorCode();
  }
#else
  struct timeval time_out = {};
  time_out.tv_usec = timeout_ms % 1000 * 1000;
  time_out.tv_sec = timeout_ms / 1000;
  if (::setsockopt(s, SOL_SOCKET, SO_SNDTIMEO, (const char *)&time_out,
                   sizeof(time_out)) < 0) {
    ec = getNetErrorCode();
  }
#endif // _WIN32
}

size_t readTimeout(socket_type s, std::error_code &ec) {
#ifdef _WIN32
  return 0;
#else
  struct timeval v = {};
  socklen_t size = sizeof(v);
  if (::getsockopt(s, SOL_SOCKET, SO_RCVTIMEO, (void *)&v, &size) < 0) {
    ec = getNetErrorCode();
  }
  return v.tv_sec * 1000 + v.tv_usec / 1000;
#endif // _WIN32
}

size_t writeTimeout(socket_type s, std::error_code &ec) {
#ifdef _WIN32
  return 0;
#else
  struct timeval v = {};
  socklen_t size = sizeof(v);
  if (::getsockopt(s, SOL_SOCKET, SO_SNDTIMEO, (void *)&v, &size) < 0) {
    ec = getNetErrorCode();
  }
  return v.tv_sec * 1000 + v.tv_usec / 1000;
#endif // _WIN32
}

int getErrorStatus(socket_type s, std::error_code &ec) {
  int set = 0;
  socklen_t ret = sizeof(set);
  if (::getsockopt(s, SOL_SOCKET, SO_ERROR, (char *)&set, &ret) < 0) {
    ec = getNetErrorCode();
    return -1;
  }
  return set;
}

int enumToNative(FamilyType family) {
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

int enumToNative(SocketType type) {
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

int enumToNative(Protocal protocal) {
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

FamilyType nativeToFamily(int family) {
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

SocketType nativeToType(int type) {
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

Protocal nativeToProtocal(int protocal) {
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

uint16_t netToHost(uint16_t v) { return ntohs(v); }

uint32_t netToHost(uint32_t v) { return ntohl(v); }

uint64_t netToHost(uint64_t v) {
  return
#ifdef _WIN32
      ntohll(v);
#else
      be64toh(v);
#endif // _WIN32
}

uint16_t hostToNet(uint16_t v) { return htons(v); }

uint32_t hostToNet(uint32_t v) { return htonl(v); }

uint64_t hostToNet(uint64_t v) {
  return
#ifdef _WIN32
      htonll(v);
#else
      htobe64(v);
#endif // _WIN32
}

} // namespace sockets
