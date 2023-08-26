
#include "proactor/ProactorCo.h"

#include "coroutine/coroutine.h"
#include "proactor/Proactor.h"
#include "proactor/operation/TcpListenerOp.h"
#include "proactor/operation/TcpStreamOp.h"
#include "proactor/operation/UdpSocketOp.h"
#include "sockets/TcpStream.h"
#include "sockets/UdpSocket.h"

Proactor *PROACTOR = nullptr;

void set_proactor(void *proactor) { PROACTOR = (Proactor *)proactor; }

socket_type co_accept(socket_type s, SocketAddr &from, std::error_code &ec) {
  TcpListenerOp op(*PROACTOR, s);
  std::error_code r_ec;
  bool ready = false;
  socket_type new_socket = {};
  op.async_accept(
      [&new_socket, &from, &ec,
       &ready](const std::error_code &re_ec,
               const std::pair<TcpStreamOp, SocketAddr> &recv) {
        new_socket = recv.first.native_handle();
        from = recv.second;
        ec = re_ec;
        ready = true;
      },
      r_ec);
  do {
    co_yield();
    std::error_code status_ec;
    if (sockets::getErrorStatus(s, status_ec) != 0 || status_ec) {
      ec = status_ec;
      return new_socket;
    }
  } while (!ready && !r_ec && PROACTOR);
  if (r_ec) {
    ec = r_ec;
  }
  return new_socket;
}

socket_type co_connect(const SocketAddr &addr, std::error_code &ec) {
  TcpStreamOp op(PROACTOR);
  std::error_code r_ec;
  bool ready = false;
  size_t recv_size;
  op.async_connect(
      addr,
      [&ec, &ready, &recv_size](const std::error_code &re_ec, size_t size) {
        ec = re_ec;
        recv_size = size;
        ready = true;
      },
      r_ec);
  socket_type s = op.native_handle();
  do {
    co_yield();
    std::error_code status_ec;
    if (sockets::getErrorStatus(s, status_ec) != 0 || status_ec) {
      ec = status_ec;
      return s;
    }
  } while (!ready && !r_ec && PROACTOR);
  if (r_ec) {
    ec = r_ec;
  }
  return s;
}

int co_tcp_read(socket_type s, char *data, size_t data_size,
                std::error_code &ec) {

  TcpStreamOp op(PROACTOR, s);
  std::error_code r_ec;
  bool ready = false;
  size_t recv_size;
  op.async_read(
      data, data_size,
      [&ec, &ready, &recv_size](const std::error_code &re_ec, size_t size) {
        ec = re_ec;
        recv_size = size;
        ready = true;
      },
      r_ec);
  do {
    co_yield();
    std::error_code status_ec;
    if (sockets::getErrorStatus(s, status_ec) != 0 || status_ec) {
      ec = status_ec;
      return -1;
    }
  } while (!ready && !r_ec && PROACTOR);
  if (r_ec) {
    ec = r_ec;
    return -1;
  }
  return ec ? -1 : (int)recv_size;
}

int co_tcp_write(socket_type s, const char *data, size_t data_size,
                 std::error_code &ec) {

  TcpStream op(s);
  return op.write(data, data_size, ec);
}

int co_udp_sendto(socket_type s, const SocketAddr &to, const char *data,
                  size_t data_size, std::error_code &ec) {

  UdpSocket op(s);
  return op.send_to(data, data_size, to, ec);
}

int co_udp_readfrom(socket_type s, SocketAddr &from, char *data,
                    size_t data_size, std::error_code &ec) {

  UdpSocketOp op(*PROACTOR, s);
  std::error_code r_ec;
  bool ready = false;
  size_t recv_size;
  op.async_read(data, data_size,
                [&from, &ec, &ready, &recv_size](const std::error_code &re_ec,
                                                 size_t size,
                                                 const SocketAddr &fr) {
                  from = fr;
                  ec = re_ec;
                  recv_size = size;
                  ready = true;
                },
                r_ec);
  do {
    co_yield();
    std::error_code status_ec;
    if (sockets::getErrorStatus(s, status_ec) != 0 || status_ec) {
      ec = status_ec;
      return -1;
    }
  } while (!ready && !r_ec && PROACTOR);
  if (r_ec) {
    ec = r_ec;
    return -1;
  }
  return ec ? -1 : (int)recv_size;
}
