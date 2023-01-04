
#ifdef _WIN32

#include "proactor/operation/TcpListenerOp.h"

#include <winsock2.h>

#include <utility>

#include "proactor/Proactor.h"
#include "sockets/TcpListener.h"
#include "utils/error_code.h"

TcpListenerOp::TcpListenerOp() : ctx_(nullptr), socket_(INVALID_SOCKET) {}

TcpListenerOp::TcpListenerOp(Proactor &context)
    : ctx_(&context), socket_(INVALID_SOCKET) {}

void TcpListenerOp::bind(const char *port_or_servicer, std::error_code &ec) {
  if (INVALID_SOCKET != socket_ && socket_ != 0) {
    return;
  }
  TcpListener listener = TcpListener::bind(port_or_servicer, ec);
  socket_ = listener.native_handle();
  if (!ec && ctx_ != nullptr) {
    ctx_->post((HANDLE)socket_, nullptr, ec);
  }
}

void TcpListenerOp::async_accept(func_type f, std::error_code &ec) {
  auto call_back = [f](void *ctx, const std::error_code &re_ec,
                       std::pair<socket_type, SocketAddr> p) {
    f(re_ec, {TcpStreamOp(static_cast<Proactor *>(ctx), p.first),
              std::move(p.second)});
  };
  accept_op_.async_accept(socket_, call_back, ec);
  if (ec) {
    accept_op_.complete(ctx_, ec, 0);
  }
}

void TcpListenerOp::close(std::error_code &ec) {
  if (!CancelIoEx((HANDLE)socket_, nullptr)) {
    std::error_code re_ec = getErrorCode();
    if (re_ec.value() != ERROR_NOT_FOUND) {
      ec = re_ec;
    }
  }
  TcpListener listener(socket_);
  listener.close(ec);
  socket_ = INVALID_SOCKET;
}

#endif // _WIN32
