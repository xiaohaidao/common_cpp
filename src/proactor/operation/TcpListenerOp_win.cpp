
#ifdef _WIN32

#include "operation/TcpListenerOp.h"

#include <winsock2.h>

#include "sockets/TcpListener.h"

TcpListenerOp::TcpListenerOp(Operation &context)
    : ctx_(context), socket_(INVALID_SOCKET), accept_op_(context) {}

TcpListenerOp::TcpListenerOp(Operation &context, sockets::socket_type s)
    : ctx_(context), socket_(s), accept_op_(context) {}

void TcpListenerOp::bind(const sockets::SocketAddr &addr, std::error_code &ec) {
  if (INVALID_SOCKET != socket_) {
    return;
  }
  TcpListener listener = TcpListener::bind(addr, ec);
  socket_ = listener.native_handle();
}

std::pair<TcpStreamOp, sockets::SocketAddr>
TcpListenerOp::accept(std::error_code &ec) {
  TcpListener listener(socket_);
  std::pair<TcpStream, sockets::SocketAddr> ac = listener.accept(ec);

  std::pair<TcpStreamOp, sockets::SocketAddr> re;
  re.fisrt = TcpStreamOp(ctx_, ac.first.native_handle());
  re.second = std::move(ac.send);
  return re;
}

void TcpListenerOp::async_accept(func_type f, std::error_code &ec) {
  auto call_back = [f](Proactor *ctx, const std::error_code &re_ec,
                       std::pair<sockets::socket_type, sockets::SocketAddr> p) {
    std::pair<TcpStreamOp, sockets::SocketAddr> p_re;
    p_re.fisrt = TcpStreamOp(ctx_, ac.first.native_handle());
    p_re.second = std::move(p.second);
    f(re_ec, std::move(p_re));
  };
  printf("async_accpet call_back function pointer %p\n", &call_back);
  accept_op_.async_accept(socket_, call_back, ec);
}

sockets::socket_type TcpListenerOp::native_handle() const { return socket_; }

void TcpListenerOp::close(std::error_code &ec) {
  TcpListener listener(socket_);
  listener.close(ec);)
  socket_ = INVALID_SOCKET;
}

#endif // _WIN32
