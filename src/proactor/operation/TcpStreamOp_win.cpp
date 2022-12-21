
#ifdef _WIN32

#include "proactor/operation/TcpStreamOp.h"

#include <winsock2.h>

#include "sockets/TcpStream.h"
#include "utils/error_code.h"

using sockets::TcpStream;

TcpStreamOp::TcpStreamOp() : ctx_(nullptr), socket_(INVALID_SOCKET) {}

TcpStreamOp::TcpStreamOp(Proactor &context)
    : ctx_(&context), socket_(INVALID_SOCKET) {}

TcpStreamOp::TcpStreamOp(Proactor &context, sockets::socket_type s)
    : ctx_(&context), socket_(s) {

  if (ctx_ != nullptr) {
    std::error_code ec;
    ctx_->post((HANDLE)socket_, ec);
    // if (ec) {
    //   LOG_WARN("overlapped post error %d %s", ec.value(), ec.message());
    // }
  }
}

void TcpStreamOp::connect(const sockets::SocketAddr &addr,
                          std::error_code &ec) {
  if (socket_ != INVALID_SOCKET && socket_ != 0) {
    close(ec);
  }
  TcpStream tcp = TcpStream::connect(addr, ec);
  socket_ = tcp.native_handle();
  if (!ec && ctx_ != nullptr) {
    ctx_->post((HANDLE)socket_, ec);
  }
}

void TcpStreamOp::async_connect(const sockets::SocketAddr &addr, func_type f,
                                std::error_code &ec) {

  if (socket_ != INVALID_SOCKET && socket_ != 0) {
    close(ec);
  }
  socket_ = sockets::socket(kUnspecified, kStream, kTCP, ec);
  if (ec) {
    return;
  }

  if (ctx_ != nullptr) {
    ctx_->post((HANDLE)socket_, ec);
  }

  auto call_back = [f](Proactor *ctx, const std::error_code &re_ec, size_t size,
                       sockets::socket_type s) {
    // assert(this->socket_ == s)
    f(re_ec, size);
  };
  connect_op_.async_connect(socket_, addr, call_back, ec);
  if (ec) {
    connect_op_.complete(ctx_, ec, 0);
  }
}

void TcpStreamOp::async_read(char *buff, size_t buff_size, func_type f,
                             std::error_code &ec) {

  auto call_back = [f](Proactor *ctx, const std::error_code &re_ec,
                       size_t recv_size) { f(re_ec, recv_size); };

  recv_op_.async_recv(socket_, buff, buff_size, call_back, ec);
  if (ec) {
    recv_op_.complete(ctx_, ec, 0);
  }
}

void TcpStreamOp::async_write(const char *buff, size_t buff_size, func_type f,
                              std::error_code &ec) {

  auto call_back = [f](Proactor *ctx, const std::error_code &re_ec,
                       size_t send_size) { f(re_ec, send_size); };

  send_op_.async_send(socket_, buff, buff_size, call_back, ec);
  if (ec) {
    send_op_.complete(ctx_, ec, 0);
  }
}

size_t TcpStreamOp::read(char *buff, size_t buff_size, std::error_code &ec) {
  TcpStream tcp(socket_);
  return tcp.read(buff, buff_size, ec);
}

size_t TcpStreamOp::write(const char *buff, size_t buff_size,
                          std::error_code &ec) {

  TcpStream tcp(socket_);
  return tcp.write(buff, buff_size, ec);
}

sockets::socket_type TcpStreamOp::native_handle() const { return socket_; }

void TcpStreamOp::close(std::error_code &ec) {
  if (!CancelIoEx((HANDLE)socket_, nullptr)) {
    std::error_code re_ec = getErrorCode();
    if (re_ec.value() != ERROR_NOT_FOUND) {
      ec = re_ec;
    }
  }
  TcpStream tcp(socket_);
  tcp.close(ec);
  socket_ = INVALID_SOCKET;
}

#endif // _WIN32