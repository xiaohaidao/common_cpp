
#include "proactor/operation/TcpListenerOp.h"

#include "proactor/Proactor.h"
#include "sockets/TcpListener.h"
#include "utils/error_code.h"

TcpListenerOp::TcpListenerOp() : ctx_(nullptr), socket_(-1) {}

TcpListenerOp::TcpListenerOp(Proactor &context) : ctx_(&context), socket_(-1) {}

TcpListenerOp::TcpListenerOp(Proactor &context, socket_type s)
    : ctx_(&context), socket_(s) {

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

TcpListenerOp::TcpListenerOp(const TcpListenerOp &other)
    : ctx_(other.ctx_), socket_(other.socket_) {}

const TcpListenerOp &TcpListenerOp::operator=(const TcpListenerOp &other) {
  this->ctx_ = other.ctx_;
  this->socket_ = other.socket_;
  this->accept_op_ = detail::AcceptOp();
  return *this;
}

std::pair<TcpStreamOp, SocketAddr> TcpListenerOp::accept(std::error_code &ec) {
  TcpListener listener(socket_);
  std::pair<TcpStream, SocketAddr> ac = listener.accept(ec);

  return {TcpStreamOp(ctx_, ac.first.native_handle()), std::move(ac.second)};
}

void TcpListenerOp::bind(const char *port_or_servicer, std::error_code &ec) {
  if (-1 != socket_ && socket_ != 0) {
    return;
  }
  TcpListener listener = TcpListener::bind(port_or_servicer, ec);
  socket_ = listener.native_handle();
#ifdef _WIN32
  if (!ec && ctx_ != nullptr) {
    ctx_->post((HANDLE)socket_, nullptr, ec); // register to io proactor
  }
#endif
}

void TcpListenerOp::async_accept(func_type f, std::error_code &ec) {
  auto call_back = [f](void *ctx, const std::error_code &re_ec,
                       std::pair<socket_type, SocketAddr> p) {
    f(re_ec, {TcpStreamOp(static_cast<Proactor *>(ctx), p.first),
              std::move(p.second)});
  };
  accept_op_.async_accept(ctx_, socket_, call_back, ec);
}

void TcpListenerOp::close(std::error_code &ec) {
  if (ctx_) {
    std::error_code t_ec;
    ctx_->cancel((::native_handle)socket_, t_ec);
  }
  TcpListener listener(socket_);
  listener.close(ec);
  socket_ = -1;
}

socket_type TcpListenerOp::native_handle() const { return socket_; }
