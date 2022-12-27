
#ifdef __linux__

#include "proactor/operation/TcpListenerOp.h"

#include <sys/socket.h>
#include <unistd.h>

#include <utility>

#include "proactor/Proactor.h"
#include "sockets/TcpListener.h"
#include "utils/error_code.h"

using sockets::SocketAddr;
using sockets::TcpListener;
using sockets::TcpStream;

TcpListenerOp::TcpListenerOp() : ctx_(nullptr), socket_(-1) {}

TcpListenerOp::TcpListenerOp(Proactor &context) : ctx_(&context), socket_(-1) {}

void TcpListenerOp::bind(const char *port_or_servicer, std::error_code &ec) {
  if (-1 != socket_ && socket_ != 0) {
    return;
  }
  TcpListener listener = TcpListener::bind(port_or_servicer, ec);
  socket_ = listener.native_handle();
}

void TcpListenerOp::async_accept(func_type f, std::error_code &ec) {
  auto call_back = [f](void *ctx, const std::error_code &re_ec,
                       std::pair<sockets::socket_type, SocketAddr> p) {
    f(re_ec, {TcpStreamOp(static_cast<Proactor *>(ctx), p.first),
              std::move(p.second)});
  };
  accept_op_.async_accept(socket_, call_back, ec);
  if (ec || !ctx_) {
    accept_op_.complete(ctx_, ec, 0);
    return;
  }
  accept_op_.set_event_data(READ_OP_ENUM);
  ctx_->post(socket_, &accept_op_, ec);
}

void TcpListenerOp::close(std::error_code &ec) {
  if (ctx_) {
    std::error_code t_ec;
    ctx_->cancel(socket_, t_ec);
  }
  TcpListener listener(socket_);
  listener.close(ec);
  socket_ = -1;
}

#endif // __linux__
