
#include "proactor/operation/IpcListenerOp.h"

#if defined(__linux__)
#include <errno.h>
#include <unistd.h>
#endif

#include "proactor/Proactor.h"
#include "sockets/TcpListener.h"
#include "utils/error_code.h"

IpcListenerOp::IpcListenerOp(Proactor &context)
    :
#if defined(_WIN32)
      ctx_(&context)
#elif defined(__linux__)
      unix_op_(context)
#endif
{
}

#if defined(_WIN32)

IpcListenerOp::IpcListenerOp(const IpcListenerOp &other)
    : ctx_(other.ctx_), pipe_(other.pipe_) {}

IpcListenerOp &IpcListenerOp::operator=(const IpcListenerOp &other) {
  if (&other == this) {
    return *this;
  }
  this->ctx_ = other.ctx_;
  this->pipe_ = other.pipe_;
  // this->connect_op_ = detail::PipeConnectOp();
  return *this;
}

#elif defined(__linux__)

IpcListenerOp::IpcListenerOp(const IpcListenerOp &other)
    : unix_op_(other.unix_op_) {}

IpcListenerOp &IpcListenerOp::operator=(const IpcListenerOp &other) {
  if (&other == this) {
    return *this;
  }
  this->unix_op_ = other.unix_op_;
  return *this;
}

#endif

void IpcListenerOp::bind(const char *port_or_servicer, std::error_code &ec) {
#if defined(_WIN32)
  pipe_ = ipc::PipeListener::create(port_or_servicer, ec);
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

IpcStreamOp IpcListenerOp::accept(std::error_code &ec) {
#if defined(_WIN32)
  return IpcStreamOp(ctx_, pipe_.accept(ec));
#elif defined(__linux__)
  return IpcStreamOp(unix_op_.accept(ec).first);
#endif
}

void IpcListenerOp::async_accept(const func_type &f, std::error_code &ec) {
#if defined(_WIN32)
  connect_op_.async_connect(
      ctx_,
      [f, &pipe = pipe_](void *ctx, const std::error_code &re_ec, size_t) {
        auto *h = pipe.native();
        std::error_code ec;
        pipe.create(ec);
        if (ctx != nullptr) {
          ((Proactor *)ctx)
              ->post((HANDLE)pipe.native(), nullptr,
                     ec); // register to io proactor
        }
        f(re_ec,
          IpcStreamOp(static_cast<Proactor *>(ctx), ipc::PipeStream(h, true)));
      },
      pipe_.native(), ec);
#elif defined(__linux__)
  unix_op_.async_accept(
      [f](const std::error_code &re_ec,
          const std::pair<TcpStreamOp, SocketAddr> &other) {
        f(re_ec, IpcStreamOp(other.first));
      },
      ec);
#endif
}

void IpcListenerOp::close(std::error_code &ec) {
#if defined(_WIN32)
  if (ctx_) {
    std::error_code t_ec;
    ctx_->cancel((::native_handle)pipe_.native(), t_ec);
  }
  pipe_.remove(ec);
#elif defined(__linux__)
  unix_op_.close(ec);
#endif
}

native_handle IpcListenerOp::native() const {
#if defined(_WIN32)
  return pipe_.native();
#elif defined(__linux__)
  return unix_op_.native();
#endif
}
