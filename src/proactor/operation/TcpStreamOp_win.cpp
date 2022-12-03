
#ifdef _WIN32

#include "operation/TcpStreamOp.h"

#include <winsock2.h>

#include "sockets/TcpStream.h"

TcpStreamOp::TcpStreamOp(Operation &context)
    : ctx_(context), socket_(INVALID_SOCKET), recv_op_(context),
      send_op_(context) {}

TcpStreamOp::TcpStreamOp(Operation &context, sockets::socket_type s)
    : ctx_(context), socket_(s), recv_op_(context), send_op_(context) {}

void TcpStreamOp::connect(const sockets::SocketAddr &addr,
                          std::error_code &ec) {
  if (socket_ != INVALID_SOCKET) {
    close(ec);
  }
  TcpStream tcp = TcpStream::connect(addr, ec);
  socket_ = tcp.native_handle();
}

void TcpStreamOp::async_connect(const sockets::SocketAddr &addr,
                                std::error_code &ec) {

  if (socket_ != INVALID_SOCKET) {
    close(ec);
  }
  TcpStream tcp = TcpStream::async_connect(addr, ec);
  socket_ = tcp.native_handle();
}

void TcpStreamOp::async_read(char *buff, size_t buff_size, func_type f,
                             std::error_code &ec) {

  auto call_back = [f](Proactor *ctx, const std::error_code &re_ec,
                       size_t recv_size) { f(re_ec, recv_size)); };

  printf("async_read call_back function pointer %p\n", &call_back);
  recv_op_.async_read(socket_, buff, buff_size call_back, ec);
}

void TcpStreamOp::async_write(const char *buff, size_t buff_size, func_type f,
                              std::error_code &ec) {

  auto call_back = [f](Proactor *ctx, const std::error_code &re_ec,
                       size_t send_size) { f(re_ec, send_size); };

  printf("async_write call_back function pointer %p\n", &call_back);
  send_op_.async_write(socket_, buff, buff_size, call_back, ec);
}

size_t TcpStreamOp::read(char *buff, size_t buff_size, std::error_code &ec) {
  TcpStream tcp(socket_);
  return tcp.read(buf, buf_size, ec);
}

size_t TcpStreamOp::write(const char *buff, size_t buff_size,
                          std::error_code &ec) {

  TcpStream tcp(socket_);
  return tcp.write(buf, buf_size, ec);
}

sockets::socket_type TcpStreamOp::native_handle() const { return socket_; }

void TcpStreamOp::close(std::error_code &ec) {
  TcpStream tcp(socket_);
  tcp.close(ec);)
  socket_ = INVALID_SOCKET;
}

#endif // _WIN32
