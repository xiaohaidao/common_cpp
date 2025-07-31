
#include "proactor/operation/TcpListenerOp.h"

#include "proactor/Proactor.h"
#include "sockets/TcpListener.h"
#include "utils/error_code.h"

tcp_listener_op::tcp_listener_op() = default;

tcp_listener_op::tcp_listener_op(proactor &context) : ctx_(&context) {}

tcp_listener_op::tcp_listener_op(proactor &context, socket_type s)
    : ctx_(&context), socket_(s) {

#ifdef _WIN32
  if (ctx_ != nullptr) {
    std::error_code ec;
    // NOLINTNEXTLINE(performance-no-int-to-ptr)
    ctx_->post((HANDLE)socket_, nullptr, ec); // register to io proactor
    // if (ec) {
    //   LOG_WARN("overlapped post error %d %s", ec.value(), ec.message());
    // }
  }
#endif
}

tcp_listener_op::tcp_listener_op(const tcp_listener_op &other) noexcept
    : ctx_(other.ctx_) {}

tcp_listener_op &tcp_listener_op::
operator=(const tcp_listener_op &other) noexcept {
  if (&other == this) {
    return *this;
  }
  this->ctx_ = other.ctx_;
  this->socket_ = other.socket_;
  // this->accept_op_ = detail::accept_op();
  return *this;
}

std::pair<tcp_stream_op, socket_addr>
tcp_listener_op::accept(std::error_code &ec) {
  tcp_listener listener(socket_);
  std::pair<tcp_stream, socket_addr> const ac = listener.accept(ec);

  return {tcp_stream_op(ctx_, ac.first.native()), ac.second};
}

void tcp_listener_op::bind(const char *port_or_servicer, std::error_code &ec) {
  return bind(port_or_servicer, kIpV4, ec);
}

void tcp_listener_op::bind(const char *port_or_servicer, FamilyType family,
                           std::error_code &ec) {
  if (-1 != socket_ && socket_ != 0) {
    return;
  }
  tcp_listener const listener =
      tcp_listener::bind(port_or_servicer, family, ec);
  socket_ = listener.native();
#ifdef _WIN32
  if (!ec && ctx_ != nullptr) {
    // NOLINTNEXTLINE(performance-no-int-to-ptr)
    ctx_->post((native_handle)socket_, nullptr, ec); // register to io proactor
  }
#endif
}

void tcp_listener_op::async_accept(const func_type &f, std::error_code &ec) {
  auto call_back = [f](void *ctx, const std::error_code &re_ec,
                       std::pair<socket_type, socket_addr> p) {
    f(re_ec, {tcp_stream_op(static_cast<proactor *>(ctx), p.first), p.second});
  };
  accept_op_.async_accept(ctx_, socket_, call_back, ec);
}

void tcp_listener_op::close(std::error_code &ec) {
  if (ctx_) {
    std::error_code t_ec;
    // NOLINTNEXTLINE(performance-no-int-to-ptr)
    ctx_->cancel((native_handle)socket_, t_ec);
  }
  tcp_listener listener(socket_);
  listener.close(ec);
  socket_ = -1;
  accept_op_ = {};
}

socket_type tcp_listener_op::native() const { return socket_; }
