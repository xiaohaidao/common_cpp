
#include "proactor/operation/IpcListenerOp.h"

#if defined(__linux__)
#include <cerrno>
#include <unistd.h>
#endif

#include "proactor/Proactor.h"
#include "sockets/TcpListener.h"
#include "utils/error_code.h"

ipc_listener_op::ipc_listener_op(proactor &context)
    :
#if defined(_WIN32)
      ctx_(&context)
#elif defined(__linux__)
      unix_op_(context)
#endif
{
}

#if defined(_WIN32)

ipc_listener_op::ipc_listener_op(const ipc_listener_op &other)
    : ctx_(other.ctx_), pipe_(other.pipe_) {}

ipc_listener_op &ipc_listener_op::operator=(const ipc_listener_op &other) {
  if (&other == this) {
    return *this;
  }
  this->ctx_ = other.ctx_;
  this->pipe_ = other.pipe_;
  // this->connect_op_ = detail::PipeConnectOp();
  return *this;
}

#elif defined(__linux__)

ipc_listener_op::ipc_listener_op(const ipc_listener_op &other) = default;

ipc_listener_op &ipc_listener_op::operator=(const ipc_listener_op &other) {
  if (&other == this) {
    return *this;
  }
  this->unix_op_ = other.unix_op_;
  return *this;
}

#endif

void ipc_listener_op::bind(const char *port_or_servicer, std::error_code &ec) {
#if defined(_WIN32)
  pipe_ = ipc::pipe_listener::create(port_or_servicer, ec);
  if (ctx_ != nullptr) {
    ctx_->post((HANDLE)pipe_.native(), nullptr, ec); // register to io proactor
  }
#elif defined(__linux__)
  // if (ec.value() == EADDRINUSE) {
  ::unlink(port_or_servicer);
  // }
  unix_op_.bind(port_or_servicer, kUnix, ec);
#endif
}

ipc_stream_op ipc_listener_op::accept(std::error_code &ec) {
#if defined(_WIN32)
  return ipc_stream_op(ctx_, pipe_.accept(ec));
#elif defined(__linux__)
  return ipc_stream_op(unix_op_.accept(ec).first);
#endif
}

void ipc_listener_op::async_accept(const func_type &f, std::error_code &ec) {
#if defined(_WIN32)
  connect_op_.async_connect(
      ctx_,
      [f, &pipe = pipe_](void *ctx, const std::error_code &re_ec, size_t) {
        auto *h = pipe.native();
        std::error_code ec;
        pipe.create(ec);
        if (ctx != nullptr) {
          ((proactor *)ctx)
              ->post((HANDLE)pipe.native(), nullptr,
                     ec); // register to io proactor
        }
        f(re_ec, ipc_stream_op(static_cast<proactor *>(ctx),
                               ipc::pipe_stream(h, true)));
      },
      pipe_.native(), ec);
#elif defined(__linux__)
  unix_op_.async_accept(
      [f](const std::error_code &re_ec,
          const std::pair<tcp_stream_op, socket_addr> &other) {
        f(re_ec, ipc_stream_op(other.first));
      },
      ec);
#endif
}

void ipc_listener_op::close(std::error_code &ec) {
#if defined(_WIN32)
  if (ctx_) {
    std::error_code t_ec;
    ctx_->cancel((::native_handle)pipe_.native(), t_ec);
  }
  pipe_.remove(ec);
  connect_op_ = {};
#elif defined(__linux__)
  unix_op_.close(ec);
#endif
}

native_handle ipc_listener_op::native() const {
#if defined(_WIN32)
  return pipe_.native();
#elif defined(__linux__)
  return unix_op_.native();
#endif
}
