// Copyright (C) 2022 All rights reserved.
// Email: oxox0@qq.com. Created in 202207

#include "sockets/SocketAddr.h"

#ifdef _WIN32
#include <winsock2.h>
#else
#include <netinet/in.h>
#include <sys/socket.h>
#endif

namespace sockets {

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

} // namespace sockets
