
/**
 * @brief
 *
 * reference https://datatracker.ietf.org/doc/html/rfc3489
 * reference https://datatracker.ietf.org/doc/html/rfc5389
 * reference https://datatracker.ietf.org/doc/html/rfc8489
 */

#include "stun/StunClient.h"

#include <string.h>

#include "sockets/TcpStream.h"
#include "stun/StunDefine.h"
#include "utils/random.h"

namespace {

void netToHost(MappedAddress &v) {
  v.family = sockets::netToHost(v.family);
  v.port = sockets::netToHost(v.port);
  v.ip_addr.ip_addr_in6[0] = sockets::netToHost(v.ip_addr.ip_addr_in6[0]);
  v.ip_addr.ip_addr_in6[1] = sockets::netToHost(v.ip_addr.ip_addr_in6[1]);
  v.ip_addr.ip_addr_in6[2] = sockets::netToHost(v.ip_addr.ip_addr_in6[2]);
  v.ip_addr.ip_addr_in6[3] = sockets::netToHost(v.ip_addr.ip_addr_in6[3]);
}

void netToHost(StunAttribute &v) {
  v.type = sockets::netToHost(v.type);
  v.length = sockets::netToHost(v.length);
}

void netToHost(StunHead &head) {
  head.message_type.short_type =
      sockets::netToHost(head.message_type.short_type);
  head.magic_cookie = sockets::netToHost(head.magic_cookie);
  head.message_length = sockets::netToHost(head.message_length);
  head.transaction_id[0] = sockets::netToHost(head.transaction_id[0]);
  head.transaction_id[1] = sockets::netToHost(head.transaction_id[1]);
  head.transaction_id[2] = sockets::netToHost(head.transaction_id[2]);
}

void hostToNet(MappedAddress &v) {
  v.family = sockets::hostToNet(v.family);
  v.port = sockets::hostToNet(v.port);
  v.ip_addr.ip_addr_in6[0] = sockets::hostToNet(v.ip_addr.ip_addr_in6[0]);
  v.ip_addr.ip_addr_in6[1] = sockets::hostToNet(v.ip_addr.ip_addr_in6[1]);
  v.ip_addr.ip_addr_in6[2] = sockets::hostToNet(v.ip_addr.ip_addr_in6[2]);
  v.ip_addr.ip_addr_in6[3] = sockets::hostToNet(v.ip_addr.ip_addr_in6[3]);
}

void hostToNet(StunAttribute &v) {
  v.type = sockets::hostToNet(v.type);
  v.length = sockets::hostToNet(v.length);
}

void hostToNet(StunHead &head) {
  head.message_type.short_type =
      sockets::hostToNet(head.message_type.short_type);
  head.magic_cookie = sockets::hostToNet(head.magic_cookie);
  head.message_length = sockets::hostToNet(head.message_length);
  head.transaction_id[0] = sockets::hostToNet(head.transaction_id[0]);
  head.transaction_id[1] = sockets::hostToNet(head.transaction_id[1]);
  head.transaction_id[2] = sockets::hostToNet(head.transaction_id[2]);
}

SocketAddr parseMapAddr(const char *buff, size_t remain) {
  MappedAddress map_addr = {};
  memcpy(&map_addr, buff, remain);
  netToHost(map_addr);

  int family = map_addr.family;
  char ip_buff[128] = {};
  char port_buff[8] = {};
  if (family == 0x01) { // 0x01 ipv4, 0x02 ipv6
    sprintf(ip_buff, "%d.%d.%d.%d", (map_addr.ip_addr.ip_addr_in >> 24 & 0xff),
            (map_addr.ip_addr.ip_addr_in >> 16 & 0xff),
            (map_addr.ip_addr.ip_addr_in >> 8 & 0xff),
            (map_addr.ip_addr.ip_addr_in >> 0 & 0xff));
    sprintf(port_buff, "%d", map_addr.port);
  } else if (family == 0x02) {
    sprintf(ip_buff, "%x:%x:%x:%x:%x:%x:%x:%x",
            (map_addr.ip_addr.ip_addr_in6[0] >> 16 & 0xffff),
            (map_addr.ip_addr.ip_addr_in6[0] >> 0 & 0xffff),
            (map_addr.ip_addr.ip_addr_in6[1] >> 16 & 0xffff),
            (map_addr.ip_addr.ip_addr_in6[1] >> 0 & 0xffff),
            (map_addr.ip_addr.ip_addr_in6[2] >> 16 & 0xffff),
            (map_addr.ip_addr.ip_addr_in6[2] >> 0 & 0xffff),
            (map_addr.ip_addr.ip_addr_in6[3] >> 16 & 0xffff),
            (map_addr.ip_addr.ip_addr_in6[3] >> 0 & 0xffff));
    sprintf(port_buff, "%d", map_addr.port);
  }
  return SocketAddr(ip_buff, port_buff);
}

SocketAddr parseXorMapAddr(const char *buff, size_t remain) {
  MappedAddress map_addr = {};
  memcpy(&map_addr, buff, remain);
  netToHost(map_addr);

  int family = map_addr.family;
  map_addr.port ^= (MAGIC_COOKIE >> 16);
  if (family == 0x02) {
    map_addr.ip_addr.ip_addr_in6[0] ^= MAGIC_COOKIE;
    map_addr.ip_addr.ip_addr_in6[1] ^= MAGIC_COOKIE;
    map_addr.ip_addr.ip_addr_in6[2] ^= MAGIC_COOKIE;
    map_addr.ip_addr.ip_addr_in6[3] ^= MAGIC_COOKIE;
  } else {
    map_addr.ip_addr.ip_addr_in ^= MAGIC_COOKIE;
  }

  hostToNet(map_addr);
  return parseMapAddr((const char *)&map_addr, sizeof(map_addr));
}

int parseAttribute(const char *buff, size_t remain) {
  if (remain < sizeof(StunAttribute)) {
    return -1;
  }
  StunAttribute attribute = {};
  memcpy(&attribute, buff, sizeof(attribute));
  netToHost(attribute);
  switch (attribute.type) {
  case kMappedAddress:
    parseMapAddr(buff + sizeof(StunAttribute), attribute.length);
    break;
  case kXorMappedAddress:
    parseXorMapAddr(buff + sizeof(StunAttribute), attribute.length);
    break;

  default:
    break;
  }

  return attribute.length + sizeof(StunAttribute);
}

} // namespace

int StunClient::request(char *buff, size_t size) {
  if (size < sizeof(StunHead)) {
    return -1;
  }
  // step 1
  StunHead head = {};
  // head.message_type.map_type.c0 = 0;
  // head.message_type.map_type.c1 = 0;
  // head.message_type.map_type.zero1 = 0;
  // head.message_type.map_type.zero2 = 0;
  head.message_type.short_type = 0x01; // Binding Request
  head.magic_cookie = MAGIC_COOKIE;
  head.transaction_id[0] = randNum();
  head.transaction_id[1] = randNum();
  head.transaction_id[2] = randNum();
  memcpy(transaction_id_, head.transaction_id, sizeof(transaction_id_));

  hostToNet(head);

  // step 2
  StunAttribute attr = {};
  attr.type = 0x01;
  ChangeRequest cr = {};
  attr.length = sizeof(cr);

  hostToNet(attr);

  // final step
  int offset = 0;
  memcpy(buff + offset, &head, sizeof(head));
  offset += sizeof(head);

  memcpy(buff + offset, &attr, sizeof(attr));
  offset += sizeof(attr);
  memcpy(buff + offset, &cr, sizeof(cr));
  offset += sizeof(cr);

  return offset;
}

int StunClient::response(const char *buff, size_t size, SocketAddr &addr) {
  if (size < sizeof(StunHead)) {
    return -1;
  }
  return parse(buff, size, addr);
}

int StunClient::parse(const char *buff, size_t size, SocketAddr &addr) {
  StunHead head;
  int offset = parse_head(buff, size, head);
  if (offset < 0) {
    return -1;
  }
  int offset_attr = 0;
  while ((size - offset) > 0) {
    if ((offset_attr =
             parse_map_attribute(buff + offset, size - offset, addr)) <= 0) {
      break;
    }
    offset += offset_attr;
  };

  if (offset_attr < 0) {
    return -1;
  }
  return size - offset;
}

int StunClient::parse_map_attribute(const char *buff, size_t size,
                                    SocketAddr &addr) {
  if (size < sizeof(StunAttribute)) {
    return -1;
  }
  StunAttribute attribute = {};
  memcpy(&attribute, buff, sizeof(attribute));
  netToHost(attribute);
  if (attribute.type != kMappedAddress && attribute.type != kXorMappedAddress) {
    return attribute.length + sizeof(StunAttribute);
  }

  addr = attribute.type == kXorMappedAddress
             ? parseXorMapAddr(buff + sizeof(attribute), attribute.length)
             : parseMapAddr(buff + sizeof(attribute), attribute.length);

  return attribute.length + sizeof(StunAttribute);
}

int StunClient::parse_head(const char *buff, size_t size, StunHead &head) {
  memcpy(&head, buff, sizeof(head));
  netToHost(head);

  if (head.message_type.map_type.zero1 != 0 ||
      head.message_type.map_type.zero2 != 0) {

    return -1;
  }

  if (head.magic_cookie != MAGIC_COOKIE) {
    return -1;
  }

  if (head.transaction_id[0] != transaction_id_[0] ||
      head.transaction_id[1] != transaction_id_[1] ||
      head.transaction_id[2] != transaction_id_[2]) {

    // return -1;
  }

  if (head.message_length != (size - sizeof(head))) {
    return -1;
  }

  return sizeof(head);
}
