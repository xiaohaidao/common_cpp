
#include "sockets/IcmpSocket.h"

#include <cstring>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <sys/socket.h>
#include <unistd.h>
#endif // _WIN32

#include "sockets/UdpSocket.h"
#include "utils/error_code.h"

#ifdef _WIN32
#define closesocket closesocket

struct icmphdr {
  uint8_t type; /* message type */
  uint8_t code; /* type sub-code */
  uint16_t checksum;
  union {
    struct {
      uint16_t id;
      uint16_t sequence;
    } echo;           /* echo datagram */
    uint32_t gateway; /* gateway address */
    struct {
      uint16_t __unused;
      uint16_t mtu;
    } frag; /* path mtu discovery */
  } un;
};

struct iphdr {
#if __BYTE_ORDER == __LITTLE_ENDIAN
  unsigned int ihl : 4;
  unsigned int version : 4;
#elif __BYTE_ORDER == __BIG_ENDIAN
  unsigned int version : 4;
  unsigned int ihl : 4;
#else
#error "Please fix <bits/endian.h>"
#endif
  uint8_t tos;
  uint16_t tot_len;
  uint16_t id;
  uint16_t frag_off;
  uint8_t ttl;
  uint8_t protocol;
  uint16_t check;
  uint32_t saddr;
  uint32_t daddr;
  /*The options start here. */
};

#define ICMP_ECHOREPLY 0       /* Echo Reply			*/
#define ICMP_DEST_UNREACH 3    /* Destination Unreachable	*/
#define ICMP_SOURCE_QUENCH 4   /* Source Quench		*/
#define ICMP_REDIRECT 5        /* Redirect (change route)	*/
#define ICMP_ECHO 8            /* Echo Request			*/
#define ICMP_TIME_EXCEEDED 11  /* Time Exceeded		*/
#define ICMP_PARAMETERPROB 12  /* Parameter Problem		*/
#define ICMP_TIMESTAMP 13      /* Timestamp Request		*/
#define ICMP_TIMESTAMPREPLY 14 /* Timestamp Reply		*/
#define ICMP_INFO_REQUEST 15   /* Information Request		*/
#define ICMP_INFO_REPLY 16     /* Information Reply		*/
#define ICMP_ADDRESS 17        /* Address Mask Request		*/
#define ICMP_ADDRESSREPLY 18   /* Address Mask Reply		*/
#define NR_ICMP_TYPES 18

#else
#define closesocket close
#define INVALID_SOCKET (socket_type)(~0)
#define SD_BOTH (SHUT_RDWR)
#endif // _WIN32

namespace {

unsigned short in_cksum(const char *addr, int len, unsigned short csum) {
  int nleft = len;
  const unsigned short *w = (const unsigned short *)addr;
  int sum = csum;

  /*
   *  Our algorithm is simple, using a 32 bit accumulator (sum),
   *  we add sequential 16 bit words to it, and at the end, fold
   *  back all the carry bits from the top 16 bits into the lower
   *  16 bits.
   */
  while (nleft > 1) {
    sum += *w++;
    nleft -= 2;
  }

  /* mop up an odd byte, if necessary */
  if (nleft == 1)
    sum +=
        *(unsigned char *)w; /* le16toh() may be unavailable on old systems */

  /*
   * add back carry outs from top 16 bits to low 16 bits
   */
  sum = (sum >> 16) + (sum & 0xffff); /* add hi 16 to low 16 */
  sum += (sum >> 16);                 /* add carry */
  return ~sum;                        /* truncate to 16 bits */
}

} // namespace

IcmpSocket::IcmpSocket()
    : socket_(INVALID_SOCKET)
#ifdef _WIN32
      ,
      read_timeout_(0), send_timeout_(0)
#endif // _WIN32
{
}

IcmpSocket::IcmpSocket(const socket_type &s)
    : socket_(s)
#ifdef _WIN32
      ,
      read_timeout_(0), send_timeout_(0)
#endif // _WIN32
{
}

IcmpSocket IcmpSocket::create(FamilyType family, std::error_code &ec) {

  IcmpSocket re;
  socket_type s =
      sockets::socket(family, kRaw, family == kIpV4 ? kIcmp : kIcmpV6, ec);
  // sockets::socket(family, kDgram, family == kIpV4 ? kIcmp : kIcmpV6, ec);
  if (ec) {
    return re;
  }
  re.socket_ = s;
  return re;
}

void IcmpSocket::set_read_timeout(size_t timeout_ms, std::error_code &ec) {
#ifdef _WIN32
  read_timeout_ = timeout_ms;
#endif // _WIN32
  sockets::setReadTimeout(socket_, timeout_ms, ec);
}

void IcmpSocket::set_write_timeout(size_t timeout_ms, std::error_code &ec) {
#ifdef _WIN32
  send_timeout_ = timeout_ms;
#endif // _WIN32
  sockets::setWriteTimeout(socket_, timeout_ms, ec);
}

size_t IcmpSocket::read_timeout(std::error_code &ec) const {
#ifdef _WIN32
  return read_timeout_;
#else
  return sockets::readTimeout(socket_, ec);
#endif // _WIN32
}

size_t IcmpSocket::write_timeout(std::error_code &ec) const {
#ifdef _WIN32
  return send_timeout_;
#else
  return sockets::writeTimeout(socket_, ec);
#endif // _WIN32
}

std::pair<size_t, SocketAddr> IcmpSocket::recv_from(char *buf, size_t buf_size,
                                                    std::error_code &ec) {

  std::pair<size_t, SocketAddr> re;
  socklen_t len = re.second.native_addr_size();
  int ret = ::recvfrom(socket_, buf, buf_size, 0,
                       (sockaddr *)re.second.native_addr(), &len);
  if (ret < 0) {
    ec = getNetErrorCode();
    return re;
  }

  struct icmphdr icmp_hdr = {};
  constexpr size_t icmp_head_size = sizeof(icmphdr);
  size_t ip_head_size = reinterpret_cast<iphdr *>(buf)->ihl * 4;
  ret -= icmp_head_size;
  ret -= ip_head_size; // ip protocal struct size
  memcpy(&icmp_hdr, buf + ip_head_size, icmp_head_size);
  memcpy(buf, buf + icmp_head_size + ip_head_size, ret);
  memset(buf + icmp_head_size + ip_head_size, 0, icmp_head_size + ip_head_size);
  if (icmp_hdr.type == ICMP_ECHOREPLY) {
    //
  }

  re.first = ret;
  return re;
}

size_t IcmpSocket::send_to(char *buf, const char *data, size_t data_size,
                           const SocketAddr &to, std::error_code &ec) {

  struct icmphdr icmp_hdr = {};
  icmp_hdr.type = ICMP_ECHO;
  icmp_hdr.un.echo.id = 1234; // arbitrary id
  memcpy(buf, &icmp_hdr, sizeof(icmp_hdr));
  memcpy(buf + sizeof(icmp_hdr), data, data_size);
  icmp_hdr.checksum = in_cksum(buf, data_size + sizeof(icmp_hdr), 0);
  memcpy(buf, &icmp_hdr, sizeof(icmp_hdr));

  int rev = ::sendto(socket_, buf, data_size + sizeof(icmp_hdr), 0,
                     (const sockaddr *)to.native_addr(), to.native_addr_size());
  if (rev < 0) {
    ec = getNetErrorCode();
    return 0;
  }
  return rev;
}

void IcmpSocket::close(std::error_code &ec) {
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

socket_type IcmpSocket::native_handle() const { return socket_; }
