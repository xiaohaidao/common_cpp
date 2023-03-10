
#include "proactor/operation/TcpStreamOp.h"

#include "sockets/TcpStream.h"
#include "utils/error_code.h"

TcpStreamOp::TcpStreamOp() : ctx_(nullptr), socket_(-1) {}

TcpStreamOp::TcpStreamOp(Proactor *context) : ctx_(context), socket_(-1) {}

TcpStreamOp::TcpStreamOp(Proactor *context, socket_type s)
    : ctx_(context), socket_(s) {

#ifdef _WIN32
  if (ctx_ != nullptr) {
    std::error_code ec;
    ctx_->post((HANDLE)socket_, nullptr, ec); // register to io proactor
    // if (ec) {
    //   LOG_WARN("overlapped post error %d %s", ec.value(), ec.message());
    // }
  }
#endif
}

TcpStreamOp::TcpStreamOp(const TcpStreamOp &other)
    : ctx_(other.ctx_), socket_(other.socket_) {}

const TcpStreamOp &TcpStreamOp::operator=(const TcpStreamOp &other) {
  this->ctx_ = other.ctx_;
  this->socket_ = other.socket_;
  this->connect_op_ = detail::ConnectOp();
  this->recv_op_ = detail::RecvOp();
  this->send_op_ = detail::SendOp();
  return *this;
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

void TcpStreamOp::connect(const SocketAddr &addr, std::error_code &ec) {
  if (socket_ != -1 && socket_ != 0) {
    close(ec);
  }
  TcpStream tcp = TcpStream::connect(addr, ec);
  socket_ = tcp.native_handle();
#ifdef _WIN32
  if (!ec && ctx_ != nullptr) {
    ctx_->post((HANDLE)socket_, nullptr, ec); // register to io proactor
  }
#endif
}

void TcpStreamOp::async_connect(const SocketAddr &addr, func_type f,
                                std::error_code &ec) {

  if (socket_ != -1 && socket_ != 0) {
    close(ec);
    if (ec) {
      return;
    }
  }
  socket_ = sockets::socket(addr.get_family(), kStream, kTCP, ec);
  if (ec) {
    return;
  }

#ifdef _WIN32
  if (ctx_ != nullptr) {
    ctx_->post((HANDLE)socket_, nullptr, ec); // register to io proactor
  }
#endif

  auto call_back = [f](void *ctx, const std::error_code &re_ec, size_t size,
                       socket_type s) {
    // assert(this->socket_ == s)
    f(re_ec, size);
  };
  connect_op_.async_connect(ctx_, socket_, addr, call_back, ec);
}

void TcpStreamOp::async_read(char *buff, size_t buff_size, func_type f,
                             std::error_code &ec) {

  auto call_back = [f](void *ctx, const std::error_code &re_ec,
                       size_t recv_size) { f(re_ec, recv_size); };

  recv_op_.async_recv(ctx_, socket_, buff, buff_size, call_back, ec);
}

void TcpStreamOp::async_write(const char *buff, size_t buff_size, func_type f,
                              std::error_code &ec) {

  auto call_back = [f](void *ctx, const std::error_code &re_ec,
                       size_t send_size) { f(re_ec, send_size); };

  send_op_.async_send(ctx_, socket_, buff, buff_size, call_back, ec);
}

void TcpStreamOp::close(std::error_code &ec) {
  if (ctx_) {
    std::error_code t_ec;
#ifdef _WIN32
    ctx_->cancel((HANDLE)socket_, t_ec);
#else
    ctx_->cancel(socket_, t_ec);
#endif
  }
  TcpStream tcp(socket_);
  tcp.close(ec);
  socket_ = -1;
}

socket_type TcpStreamOp::native_handle() const { return socket_; }
