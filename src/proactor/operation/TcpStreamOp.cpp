
#include "proactor/operation/TcpStreamOp.h"

#ifdef __linux__
#include <unistd.h>
#endif

#include "sockets/TcpStream.h"
#include "utils/error_code.h"

TcpStreamOp::TcpStreamOp()
    : ctx_(nullptr), socket_(-1)
#ifdef __linux__
      ,
      write_socket_(-1)
#endif
{
}

TcpStreamOp::TcpStreamOp(Proactor *context)
    : ctx_(context), socket_(-1)
#ifdef __linux__
      ,
      write_socket_(-1)
#endif
{
}

TcpStreamOp::TcpStreamOp(Proactor *context, socket_type s)
    : ctx_(context), socket_(s)
#ifdef __linux__
      ,
      write_socket_(-1)
#endif
{

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

TcpStreamOp::TcpStreamOp(const TcpStreamOp &other)
    : ctx_(other.ctx_), socket_(other.socket_)
#ifdef __linux__
      ,
      write_socket_(other.write_socket_)
#endif
{
}

TcpStreamOp &TcpStreamOp::operator=(const TcpStreamOp &other) {
  if (&other == this) {
    return *this;
  }
  this->ctx_ = other.ctx_;
  this->socket_ = other.socket_;
#ifdef __linux__
  this->write_socket_ = other.write_socket_;
#endif
  // this->connect_op_ = detail::ConnectOp();
  // this->recv_op_ = detail::RecvOp();
  // this->send_op_ = detail::SendOp();
  return *this;
}

size_t TcpStreamOp::read(char *buff, size_t buff_size, std::error_code &ec) {
  TcpStream tcp(socket_);
  return tcp.read(buff, buff_size, ec);
}

size_t TcpStreamOp::write(const char *buff, size_t buff_size,
                          std::error_code &ec) {

  TcpStream tcp(socket_);
  return tcp.write(buff, buff_size, ec);
}

void TcpStreamOp::connect(const SocketAddr &addr, std::error_code &ec) {
  if (socket_ != -1 && socket_ != 0) {
    close(ec);
  }
  TcpStream const tcp = TcpStream::connect(addr, ec);
  socket_ = tcp.native();
#ifdef _WIN32
  if (!ec && ctx_ != nullptr) {
    // NOLINTNEXTLINE(performance-no-int-to-ptr)
    ctx_->post((HANDLE)socket_, nullptr, ec); // register to io proactor
  }
#endif
}

void TcpStreamOp::async_connect(const SocketAddr &addr, const func_type &f,
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

void TcpStreamOp::async_read(char *buff, size_t buff_size, const func_type &f,
                             std::error_code &ec) {

  auto call_back = [f](void * /*ctx*/, const std::error_code &re_ec,
                       size_t recv_size) { f(re_ec, recv_size); };

  recv_op_.async_recv(ctx_, socket_, buff, buff_size, call_back, ec);
}

void TcpStreamOp::async_write(const char *buff, size_t buff_size,
                              const func_type &f, std::error_code &ec) {

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

void TcpStreamOp::shutdown(std::error_code &ec) {
  TcpStream tcp(socket_);
  tcp.shutdown(ec);
}

void TcpStreamOp::close(std::error_code &ec) {
  if (ctx_) {
    std::error_code t_ec;
    // NOLINTNEXTLINE(performance-no-int-to-ptr)
    ctx_->cancel((::native_handle)socket_, t_ec);
  }
  TcpStream tcp(socket_);
  tcp.close(ec);
  socket_ = -1;
#ifdef __linux__
  ::close(write_socket_);
  write_socket_ = -1;
#endif
}

socket_type TcpStreamOp::native() const { return socket_; }
