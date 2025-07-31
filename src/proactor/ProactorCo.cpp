
#include "proactor/ProactorCo.h"

#include "coroutine/coroutine.h"
#include "proactor/Proactor.h"
#include "proactor/operation/TcpListenerOp.h"
#include "proactor/operation/TcpStreamOp.h"
#include "proactor/operation/UdpSocketOp.h"
#include "sockets/TcpStream.h"
#include "sockets/UdpSocket.h"

proactor *local_proactor = nullptr;

void set_proactor(void *p) { local_proactor = (proactor *)p; }

socket_type co_accept(socket_type s, socket_addr &from, std::error_code &ec) {
  tcp_listener_op op(*local_proactor, s);
  std::error_code r_ec;
  bool ready = false;
  socket_type new_socket = {};
  op.async_accept(
      [&new_socket, &from, &ec,
       &ready](const std::error_code &re_ec,
               const std::pair<tcp_stream_op, socket_addr> &recv) {
        new_socket = recv.first.native();
        from = recv.second;
        ec = re_ec;
        ready = true;
      },
      r_ec);
  do {
    CO_YIELD();
    std::error_code status_ec;
    if (sockets::get_error_status(s, status_ec) != 0 || status_ec) {
      ec = status_ec;
      return new_socket;
    }
  } while (!ready && !r_ec && local_proactor);
  if (r_ec) {
    ec = r_ec;
  }
  return new_socket;
}

socket_type co_connect(const socket_addr &addr, std::error_code &ec) {
  tcp_stream_op op(local_proactor);
  std::error_code r_ec;
  bool ready = false;
  size_t recv_size = 0;
  op.async_connect(
      addr,
      [&ec, &ready, &recv_size](const std::error_code &re_ec, size_t size) {
        ec = re_ec;
        recv_size = size;
        ready = true;
      },
      r_ec);
  socket_type const s = op.native();
  do {
    CO_YIELD();
    std::error_code status_ec;
    if (sockets::get_error_status(s, status_ec) != 0 || status_ec) {
      ec = status_ec;
      return s;
    }
  } while (!ready && !r_ec && local_proactor);
  if (r_ec) {
    ec = r_ec;
  }
  return s;
}

int co_tcp_read(socket_type s, char *data, size_t data_size,
                std::error_code &ec) {

  tcp_stream_op op(local_proactor, s);
  std::error_code r_ec;
  bool ready = false;
  size_t recv_size = 0;
  op.async_read(
      data, data_size,
      [&ec, &ready, &recv_size](const std::error_code &re_ec, size_t size) {
        ec = re_ec;
        recv_size = size;
        ready = true;
      },
      r_ec);
  do {
    CO_YIELD();
    std::error_code status_ec;
    if (sockets::get_error_status(s, status_ec) != 0 || status_ec) {
      ec = status_ec;
      return -1;
    }
  } while (!ready && !r_ec && local_proactor);
  if (r_ec) {
    ec = r_ec;
    return -1;
  }
  return ec ? -1 : (int)recv_size;
}

int co_tcp_write(socket_type s, const char *data, size_t data_size,
                 std::error_code &ec) {

  tcp_stream op(s);
  return op.write(data, data_size, ec);
}

int co_udp_sendto(socket_type s, const socket_addr &to, const char *data,
                  size_t data_size, std::error_code &ec) {

  udp_socket op(s);
  return op.send_to(data, data_size, to, ec);
}

int co_udp_readfrom(socket_type s, socket_addr &from, char *data,
                    size_t data_size, std::error_code &ec) {

  udp_socket_op op(*local_proactor, s);
  std::error_code r_ec;
  bool ready = false;
  size_t recv_size = 0;
  op.async_read(data, data_size,
                [&from, &ec, &ready, &recv_size](const std::error_code &re_ec,
                                                 size_t size,
                                                 const socket_addr &fr) {
                  from = fr;
                  ec = re_ec;
                  recv_size = size;
                  ready = true;
                },
                r_ec);
  do {
    CO_YIELD();
    std::error_code status_ec;
    if (sockets::get_error_status(s, status_ec) != 0 || status_ec) {
      ec = status_ec;
      return -1;
    }
  } while (!ready && !r_ec && local_proactor);
  if (r_ec) {
    ec = r_ec;
    return -1;
  }
  return ec ? -1 : (int)recv_size;
}
