
#ifdef __linux__

#include "proactor/operation/TcpStreamOp.h"

#include <sys/socket.h>
#include <unistd.h>

#include "sockets/TcpStream.h"
#include "utils/error_code.h"

TcpStreamOp::TcpStreamOp() : ctx_(nullptr), socket_(-1) {}

TcpStreamOp::TcpStreamOp(Proactor *context) : ctx_(context), socket_(-1) {}

TcpStreamOp::TcpStreamOp(Proactor *context, socket_type s)
    : ctx_(context), socket_(s) {}

void TcpStreamOp::connect(const SocketAddr &addr, std::error_code &ec) {
  if (socket_ != -1 && socket_ != 0) {
    close(ec);
  }
  TcpStream tcp = TcpStream::connect(addr, ec);
  socket_ = tcp.native_handle();
}

void TcpStreamOp::async_connect(const SocketAddr &addr, func_type f,
                                std::error_code &ec) {

  if (socket_ != -1 && socket_ != 0) {
    close(ec);
  }
  socket_ = sockets::socket(addr.get_family(), kStream, kTCP, ec);
  if (ec) {
    return;
  }

  auto call_back = [f](void *ctx, const std::error_code &re_ec, size_t size,
                       socket_type s) {
    // assert(this->socket_ == s)
    f(re_ec, size);
  };
  connect_op_.async_connect(socket_, addr, call_back, ec);
  if (ec || !ctx_) {
    connect_op_.complete(ctx_, ec, 0);
    return;
  }
  connect_op_.set_event_data(WRITE_OP_ENUM);
  ctx_->post(socket_, &connect_op_, ec);
}

void TcpStreamOp::async_read(char *buff, size_t buff_size, func_type f,
                             std::error_code &ec) {

  auto call_back = [f](void *ctx, const std::error_code &re_ec,
                       size_t recv_size) { f(re_ec, recv_size); };

  recv_op_.async_recv(socket_, buff, buff_size, call_back, ec);
  if (ec || !ctx_) {
    recv_op_.complete(ctx_, ec, 0);
    return;
  }
  recv_op_.set_event_data(READ_OP_ENUM);
  ctx_->post(socket_, &recv_op_, ec);
}

void TcpStreamOp::async_write(const char *buff, size_t buff_size, func_type f,
                              std::error_code &ec) {

  auto call_back = [f](void *ctx, const std::error_code &re_ec,
                       size_t send_size) { f(re_ec, send_size); };

  send_op_.async_send(socket_, buff, buff_size, call_back, ec);
  if (ec || !ctx_) {
    send_op_.complete(ctx_, ec, 0);
    return;
  }
  send_op_.set_event_data(WRITE_OP_ENUM);
  ctx_->post(socket_, &send_op_, ec);
}

void TcpStreamOp::close(std::error_code &ec) {
  if (ctx_) {
    std::error_code t_ec;
    ctx_->cancel(socket_, t_ec);
  }
  TcpStream tcp(socket_);
  tcp.close(ec);
  socket_ = -1;
}

#endif // __linux__
