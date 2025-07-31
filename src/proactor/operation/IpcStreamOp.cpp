
#include "proactor/operation/IpcStreamOp.h"

#include <utility>

#include "sockets/TcpStream.h"
#include "utils/error_code.h"

ipc_stream_op::ipc_stream_op(proactor *context)
    :
#if defined(_WIN32)
      ctx_(context)
#elif defined(__linux__)
      tcp_(context)
#endif
{
}

#if defined(_WIN32)
ipc_stream_op::ipc_stream_op(proactor *context, const ipc::pipe_stream &pipe)
    : ctx_(context), pipe_(pipe) {}

#elif defined(__linux__)
ipc_stream_op::ipc_stream_op(const tcp_stream_op &tcp) : tcp_(tcp) {}
#endif

#if defined(_WIN32)

ipc_stream_op::ipc_stream_op(const ipc_stream_op &other)
    : ctx_(other.ctx_), pipe_(other.pipe_) {}

ipc_stream_op &ipc_stream_op::operator=(const ipc_stream_op &other) {
  if (&other == this) {
    return *this;
  }
  this->ctx_ = other.ctx_;
  this->pipe_ = other.pipe_;
  // this->read_ = detail::ReadOp();
  // this->write_ = detail::WriteOp();
  return *this;
}

#elif defined(__linux__)

ipc_stream_op::ipc_stream_op(const ipc_stream_op &other) = default;

ipc_stream_op &ipc_stream_op::operator=(const ipc_stream_op &other) {
  if (&other == this) {
    return *this;
  }
  this->tcp_ = other.tcp_;
  return *this;
}

#endif

size_t ipc_stream_op::read(char *buff, size_t buff_size, std::error_code &ec) {
#if defined(_WIN32)
  return pipe_.read(buff, buff_size, ec);
#elif defined(__linux__)
  return tcp_.read(buff, buff_size, ec);
#endif
}

size_t ipc_stream_op::write(const char *buff, size_t buff_size,
                            std::error_code &ec) {

#if defined(_WIN32)
  return pipe_.write(buff, buff_size, ec);
#elif defined(__linux__)
  return tcp_.write(buff, buff_size, ec);
#endif
}

void ipc_stream_op::connect(const char *buff, std::error_code &ec) {
#if defined(_WIN32)
  pipe_ = ipc::pipe_stream::connect(buff, ec);
  if (ctx_ != nullptr) {
    // NOLINTNEXTLINE(performance-no-int-to-ptr)
    ctx_->post((HANDLE)pipe_.native(), nullptr, ec); // register to io proactor
  }
#elif defined(__linux__)
  socket_addr addr(buff);
  tcp_.connect(addr, ec);
#endif
}

void ipc_stream_op::async_read(char *buff, size_t buff_size, const func_type &f,
                               std::error_code &ec) {

#if defined(_WIN32)
  read_.async_read(ctx_,
                   [f](void * /*ctx*/, const std::error_code &re_ec,
                       size_t recv_size) { f(re_ec, recv_size); },
                   pipe_.native(), buff, buff_size, ec);
#elif defined(__linux__)
  tcp_.async_read(buff, buff_size, f, ec);
#endif
}

void ipc_stream_op::async_write(const char *buff, size_t buff_size,
                                const func_type &f, std::error_code &ec) {

#if defined(_WIN32)
  write_.async_write(ctx_,
                     [f](void * /*ctx*/, const std::error_code &re_ec,
                         size_t recv_size) { f(re_ec, recv_size); },
                     pipe_.native(), buff, buff_size, ec);
#elif defined(__linux__)
  tcp_.async_write(buff, buff_size, f, ec);
#endif
}

void ipc_stream_op::close(std::error_code &ec) {
#if defined(_WIN32)
  if (ctx_) {
    std::error_code t_ec;
    ctx_->cancel(pipe_.native(), t_ec);
  }
  pipe_.close(ec);
  read_ = {};
  write_ = {};
#elif defined(__linux__)
  tcp_.shutdown(ec);
  tcp_.close(ec);
#endif
}

native_handle ipc_stream_op::native() const {
#if defined(_WIN32)
  return pipe_.native();
#elif defined(__linux__)
  return tcp_.native();
#endif
}
