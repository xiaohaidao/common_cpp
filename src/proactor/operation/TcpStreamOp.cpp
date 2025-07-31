
#include "proactor/operation/TcpStreamOp.h"

#ifdef __linux__
#include <unistd.h>
#endif

#include "sockets/TcpStream.h"
#include "utils/error_code.h"

tcp_stream_op::tcp_stream_op(proactor *context) : ctx_(context) {}

tcp_stream_op::tcp_stream_op(proactor *context, socket_type s)
    : ctx_(context), socket_(s) {

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

tcp_stream_op::tcp_stream_op(const tcp_stream_op &other)
    : ctx_(other.ctx_), socket_(other.socket_)
#ifdef __linux__
      ,
      write_socket_(other.write_socket_)
#endif
{
}

tcp_stream_op &tcp_stream_op::operator=(const tcp_stream_op &other) {
  if (&other == this) {
    return *this;
  }
  this->ctx_ = other.ctx_;
  this->socket_ = other.socket_;
#ifdef __linux__
  this->write_socket_ = other.write_socket_;
#endif
  // this->connect_op_ = detail::ConnectOp();
  // this->recv_op_ = detail::recv_op();
  // this->send_op_ = detail::send_op();
  return *this;
}

size_t tcp_stream_op::read(char *buff, size_t buff_size, std::error_code &ec) {
  tcp_stream tcp(socket_);
  return tcp.read(buff, buff_size, ec);
}

size_t tcp_stream_op::write(const char *buff, size_t buff_size,
                            std::error_code &ec) {

  tcp_stream tcp(socket_);
  return tcp.write(buff, buff_size, ec);
}

void tcp_stream_op::connect(const socket_addr &addr, std::error_code &ec) {
  if (socket_ != -1 && socket_ != 0) {
    close(ec);
  }
  tcp_stream const tcp = tcp_stream::connect(addr, ec);
  socket_ = tcp.native();
#ifdef _WIN32
  if (!ec && ctx_ != nullptr) {
    // NOLINTNEXTLINE(performance-no-int-to-ptr)
    ctx_->post((HANDLE)socket_, nullptr, ec); // register to io proactor
  }
#endif
}

void tcp_stream_op::async_connect(const socket_addr &addr,
                                  const tcp_stream_op::func_type &f,
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
    // NOLINTNEXTLINE(performance-no-int-to-ptr)
    ctx_->post((HANDLE)socket_, nullptr, ec); // register to io proactor
  }
#endif

  auto call_back = [f](void * /*ctx*/, const std::error_code &re_ec,
                       size_t size, socket_type /*s*/) {
    // assert(this->socket_ == s)
    f(re_ec, size);
  };
  connect_op_.async_connect(ctx_, socket_, addr, call_back, ec);
}

void tcp_stream_op::async_read(char *buff, size_t buff_size,
                               const tcp_stream_op::func_type &f,
                               std::error_code &ec) {

  auto call_back = [f](void * /*ctx*/, const std::error_code &re_ec,
                       size_t recv_size) { f(re_ec, recv_size); };

  recv_op_.async_recv(ctx_, socket_, buff, buff_size, call_back, ec);
}

void tcp_stream_op::async_write(const char *buff, size_t buff_size,
                                const tcp_stream_op::func_type &f,
                                std::error_code &ec) {

#ifdef __linux__
  if (write_socket_ == -1) {
    write_socket_ = ::dup(socket_);
  }
  auto s = write_socket_;
#else
  auto s = socket_;
#endif
  auto call_back = [f](void * /*ctx*/, const std::error_code &re_ec,
                       size_t send_size) { f(re_ec, send_size); };

  send_op_.async_send(ctx_, s, buff, buff_size, call_back, ec);
}

void tcp_stream_op::shutdown(std::error_code &ec) {
  tcp_stream tcp(socket_);
  tcp.shutdown(ec);
}

void tcp_stream_op::close(std::error_code &ec) {
  if (ctx_) {
    std::error_code t_ec;
    // NOLINTNEXTLINE(performance-no-int-to-ptr)
    ctx_->cancel((::native_handle)socket_, t_ec);
  }
  tcp_stream tcp(socket_);
  tcp.close(ec);
  socket_ = -1;
#ifdef __linux__
  ::close(write_socket_);
  write_socket_ = -1;
#endif
  send_op_ = {};
  connect_op_ = {};
  recv_op_ = {};
}

socket_type tcp_stream_op::native() const { return socket_; }
