// Copyright (C) 2022 All rights reserved.
// Email: oxox0@qq.com. Created in 202207

#include "sockets/UdpSocket.h"

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#endif // _WIN32

#include "utils/error_code.h"

#ifdef _WIN32
#define closesocket closesocket
#else
#define closesocket close
#define INVALID_SOCKET (socket_type)(~0)
#define SD_BOTH (SHUT_RDWR)
#endif // _WIN32

UdpSocket::UdpSocket()
    : socket_(INVALID_SOCKET)
#ifdef _WIN32
      ,
      read_timeout_(0), send_timeout_(0)
#endif // _WIN32
{
}

UdpSocket::UdpSocket(const socket_type &s)
    : socket_(s)
#ifdef _WIN32
      ,
      read_timeout_(0), send_timeout_(0)
#endif // _WIN32
{
}

UdpSocket UdpSocket::create(FamilyType family, std::error_code &ec) {

  UdpSocket re;
  socket_type s = sockets::socket(family, kDgram, kUDP, ec);
  if (ec) {
    return re;
  }
  re.socket_ = s;
  return re;
}

UdpSocket UdpSocket::bind(const char *port_or_service, std::error_code &ec) {
  return bind(port_or_service, kIpV4, ec);
}

UdpSocket UdpSocket::bind(const char *port_or_service, FamilyType family,
                          std::error_code &ec) {
  UdpSocket re;
  socket_type s = sockets::socket(family, kDgram, kUDP, ec);
  if (ec) {
    return re;
  }
  re.socket_ = s;
  sockets::setReuseAddr(s, ec);
  if (ec) {
    ::closesocket(s);
    return re;
  }

  SocketAddr addr =
      SocketAddr::resolve_host(nullptr, port_or_service, ec, family, true);
  if (ec) {
    ::closesocket(s);
    return re;
  }

  if (::bind(s, (sockaddr *)addr.native_addr(), addr.native_addr_size())) {
    ::closesocket(s);
    ec = getNetErrorCode();
    return re;
  }
  return re;
}

void UdpSocket::set_read_timeout(size_t timeout_ms, std::error_code &ec) {
#ifdef _WIN32
  read_timeout_ = timeout_ms;
#endif // _WIN32
  sockets::setReadTimeout(socket_, timeout_ms, ec);
}

void UdpSocket::set_write_timeout(size_t timeout_ms, std::error_code &ec) {
#ifdef _WIN32
  send_timeout_ = timeout_ms;
#endif // _WIN32
  sockets::setWriteTimeout(socket_, timeout_ms, ec);
}

size_t UdpSocket::read_timeout(std::error_code &ec) const {
#ifdef _WIN32
  return read_timeout_;
#else
  return sockets::readTimeout(socket_, ec);
#endif // _WIN32
}

size_t UdpSocket::write_timeout(std::error_code &ec) const {
#ifdef _WIN32
  return send_timeout_;
#else
  return sockets::writeTimeout(socket_, ec);
#endif // _WIN32
}

std::pair<int, SocketAddr> UdpSocket::recv_from(char *buf, size_t buf_size,
                                                std::error_code &ec) {

  std::pair<int, SocketAddr> re;
  socklen_t len = re.second.native_addr_size();
  re.first = ::recvfrom(socket_, buf, buf_size, 0,
                        (sockaddr *)re.second.native_addr(), &len);
  if (re.first < 0) {
    ec = getNetErrorCode();
  }
  return re;
}

int UdpSocket::send_to(const char *buf, size_t buf_size, const SocketAddr &to,
                       std::error_code &ec) {

  int rev = ::sendto(socket_, buf, buf_size, 0,
                     (const sockaddr *)to.native_addr(), to.native_addr_size());
  if (rev < 0) {
    ec = getNetErrorCode();
  }
  return rev;
}

void UdpSocket::close(std::error_code &ec) {
  if (::shutdown(socket_, SD_BOTH)) {
    std::error_code re_ec = getNetErrorCode();
    if (ENOTCONN != re_ec.value()) {
      ec = re_ec;
    }
  }
  if (::closesocket(socket_)) {
    ec = getNetErrorCode();
  }
}
void UdpSocket::joint_multicast(const SocketAddr &multicast,
                                std::error_code &ec) {

  struct ip_mreq mreq = {};
  mreq.imr_multiaddr =
      ((struct sockaddr_in *)multicast.native_addr())->sin_addr;
  // mreq.imr_interface =
  //     ((struct sockaddr_in *)interface.native_addr())->sin_addr;
  if (::setsockopt(socket_, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *)&mreq,
                   sizeof(mreq)) < 0) {
    ec = getNetErrorCode();
  }
}

void UdpSocket::leave_multicast(const SocketAddr &multicast,
                                std::error_code &ec) {
  struct ip_mreq mreq = {};
  mreq.imr_multiaddr =
      ((struct sockaddr_in *)multicast.native_addr())->sin_addr;
  if (::setsockopt(socket_, IPPROTO_IP, IP_DROP_MEMBERSHIP, (char *)&mreq,
                   sizeof(mreq)) < 0) {
    ec = getNetErrorCode();
  }
}

void UdpSocket::set_multicast_loop(bool enable, std::error_code &ec) {
  int en = enable ? 1 : 0;
  if (::setsockopt(socket_, IPPROTO_IP, IP_MULTICAST_LOOP, (char *)&en,
                   sizeof(en)) < 0) {
    ec = getNetErrorCode();
  }
}

bool UdpSocket::multicast_loop(std::error_code &ec) {
  int en = 0;
  socklen_t len = sizeof(en);
  if (::getsockopt(socket_, IPPROTO_IP, IP_MULTICAST_LOOP, (char *)&en, &len) <
      0) {
    ec = getNetErrorCode();
  }
  return en;
}

void UdpSocket::set_multicast_ttl(int ttl, std::error_code &ec) {
  if (::setsockopt(socket_, IPPROTO_IP, IP_MULTICAST_TTL, (char *)&ttl,
                   sizeof(ttl)) < 0) {
    ec = getNetErrorCode();
  }
}

int UdpSocket::multicast_ttl(std::error_code &ec) {
  int en = 0;
  socklen_t len = sizeof(en);
  if (::getsockopt(socket_, IPPROTO_IP, IP_MULTICAST_TTL, (char *)&en, &len) <
      0) {
    ec = getNetErrorCode();
  }
  return en;
}

void UdpSocket::joint_multicast_v6(const SocketAddr &multicast,
                                   std::error_code &ec) {

  struct ipv6_mreq mreq = {};
  mreq.ipv6mr_multiaddr =
      ((struct sockaddr_in6 *)multicast.native_addr())->sin6_addr;
  if (::setsockopt(socket_, IPPROTO_IPV6, IPV6_ADD_MEMBERSHIP, (char *)&mreq,
                   sizeof(mreq)) < 0) {
    ec = getNetErrorCode();
  }
}

void UdpSocket::leave_multicast_v6(const SocketAddr &multicast,
                                   std::error_code &ec) {
  struct ipv6_mreq mreq = {};
  mreq.ipv6mr_multiaddr =
      ((struct sockaddr_in6 *)multicast.native_addr())->sin6_addr;
  if (::setsockopt(socket_, IPPROTO_IPV6, IPV6_DROP_MEMBERSHIP, (char *)&mreq,
                   sizeof(mreq)) < 0) {
    ec = getNetErrorCode();
  }
}

void UdpSocket::set_multicast_loop_v6(bool enable, std::error_code &ec) {
  int en = enable ? 1 : 0;
  if (::setsockopt(socket_, IPPROTO_IPV6, IPV6_MULTICAST_LOOP, (char *)&en,
                   sizeof(en)) < 0) {
    ec = getNetErrorCode();
  }
}

bool UdpSocket::multicast_loop_v6(std::error_code &ec) {
  int en = 0;
  socklen_t len = sizeof(en);
  if (::getsockopt(socket_, IPPROTO_IPV6, IPV6_MULTICAST_LOOP, (char *)&en,
                   &len) < 0) {
    ec = getNetErrorCode();
  }
  return en;
}

socket_type UdpSocket::native_handle() const { return socket_; }