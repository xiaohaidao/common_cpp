
#include "proactor/operation/TcpListenerOp.h"

#include "proactor/Proactor.h"
#include "sockets/TcpListener.h"
#include "utils/error_code.h"

using sockets::SocketAddr;
using sockets::TcpListener;
using sockets::TcpStream;

TcpListenerOp::TcpListenerOp(Proactor &context, sockets::socket_type s)
    : ctx_(&context), socket_(s) {}

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

sockets::socket_type TcpListenerOp::native_handle() const { return socket_; }
