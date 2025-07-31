
#ifndef PROACTOR_PROACTORCO_H
#define PROACTOR_PROACTORCO_H

#include "sockets/SocketAddr.h"

// set nullptr when need to stop
void set_proactor(void *proactor);

socket_type co_accept(socket_type s, socket_addr &from, std::error_code &ec);

socket_type co_connect(const socket_addr &addr, std::error_code &ec);

int co_tcp_read(socket_type s, char *data, size_t data_size,
                std::error_code &ec);

int co_tcp_write(socket_type s, const char *data, size_t data_size,
                 std::error_code &ec);

int co_udp_sendto(socket_type s, const socket_addr &to, const char *data,
                  size_t data_size, std::error_code &ec);

int co_udp_readfrom(socket_type s, socket_addr &from, char *data,
                    size_t data_size, std::error_code &ec);

#endif // PROACTOR_PROACTORCO_H
