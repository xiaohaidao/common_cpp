
#ifndef STUN_STUNCLIENT_H
#define STUN_STUNCLIENT_H

#include <stddef.h>

struct StunHead;
class SocketAddr;

class StunClient {
public:
  // return -1 when error
  int request(char *buff, size_t size);
  int response(const char *buff, size_t size, SocketAddr &public_addr);

private:
  int parse(const char *buff, size_t size, SocketAddr &addr);
  int parse_head(const char *buff, size_t size, StunHead &head);
  int parse_map_attribute(const char *buff, size_t size, SocketAddr &addr);

  unsigned int transaction_id_[3];
};

#endif // STUN_STUNCLIENT_H
