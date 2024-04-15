
#include "proactor/operation/UdpSocketOp.h"

#ifdef __linux__
#include <unistd.h>
#endif

#include "sockets/UdpSocket.h"
#include "utils/error_code.h"

UdpSocketOp::UdpSocketOp()
    : ctx_(nullptr), socket_(-1)
#ifdef __linux__
      ,
      write_socket_(-1)
#endif
{
}

UdpSocketOp::UdpSocketOp(Proactor &context)
    : ctx_(&context), socket_(-1)
#ifdef __linux__
      ,
      write_socket_(-1)
#endif
{
}

UdpSocketOp::UdpSocketOp(Proactor &context, socket_type s)
    : ctx_(&context), socket_(s)
#ifdef __linux__
      ,
      write_socket_(-1)
#endif
{

#ifdef _WIN32
  if (ctx_ != nullptr) {
    std::error_code ec;
    // NOLINTNEXTLINE(performance-no-int-to-ptr)
    ctx_->post((native_handle)socket_, nullptr, ec); // register to io proactor
    // if (ec) {
    //   LOG_WARN("overlapped post error %d %s", ec.value(), ec.message());
    // }
  }
#endif
}

UdpSocketOp::UdpSocketOp(const UdpSocketOp &other)
    : ctx_(other.ctx_), socket_(other.socket_)
#ifdef __linux__
      ,
      write_socket_(other.write_socket_)
#endif
{
}

UdpSocketOp &UdpSocketOp::operator=(const UdpSocketOp &other) {
  if (&other == this) {
    return *this;
  }
  this->ctx_ = other.ctx_;
  this->socket_ = other.socket_;
#ifdef __linux__
  this->write_socket_ = other.write_socket_;
#endif
  // this->recvfrom_op_ = detail::RecvFromOp();
  // this->sendto_op_ = detail::SendToOp();
  return *this;
}

std::pair<size_t, SocketAddr>
UdpSocketOp::recv_from(char *buff, size_t buff_size, std::error_code &ec) {

  UdpSocket tcp(socket_);
  return tcp.recv_from(buff, buff_size, ec);
}

size_t UdpSocketOp::send_to(const char *buff, size_t buff_size,
                            const SocketAddr &to, std::error_code &ec) {

  UdpSocket tcp(socket_);
  return tcp.send_to(buff, buff_size, to, ec);
}

void UdpSocketOp::async_read(char *buff, size_t buff_size,
                             const func_recv_type &f, std::error_code &ec) {

  auto call_back = [f](void * /*ctx*/, const std::error_code &re_ec,
                       size_t recv_size,
                       const SocketAddr &from) { f(re_ec, recv_size, from); };

  recvfrom_op_.async_recv_from(ctx_, socket_, buff, buff_size, call_back, ec);
}

void UdpSocketOp::async_write(const char *buff, size_t buff_size,
                              const SocketAddr &to, const func_send_type &f,
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

  sendto_op_.async_send_to(ctx_, s, buff, buff_size, to, call_back, ec);
}

void UdpSocketOp::create(FamilyType family, std::error_code &ec) {
  if (socket_ > 0) {
    std::error_code t_ec;
    close(t_ec);
  }
  UdpSocket const udp = UdpSocket::create(family, ec);
  if (ec) {
    return;
  }
  socket_ = udp.native();
#ifdef _WIN32
  if (ctx_ != nullptr) {
    std::error_code ec;
    // NOLINTNEXTLINE(performance-no-int-to-ptr)
    ctx_->post((::native_handle)socket_, nullptr,
               ec); // register to io proactor
  }
#endif
}

void UdpSocketOp::bind(const char *port_or_service, std::error_code &ec) {
  bind(port_or_service, kIpV4, ec);
}

void UdpSocketOp::bind(const char *port_or_service, FamilyType family,
                       std::error_code &ec) {
  if (socket_ > 0) {
    std::error_code t_ec;
    close(t_ec);
  }
  UdpSocket const udp = UdpSocket::bind(port_or_service, family, ec);
  if (ec) {
    return;
  }
  socket_ = udp.native();
#ifdef _WIN32
  if (ctx_ != nullptr) {
    std::error_code ec;
    // NOLINTNEXTLINE(performance-no-int-to-ptr)
    ctx_->post((::native_handle)socket_, nullptr,
               ec); // register to io proactor
  }
#endif
}

void UdpSocketOp::close(std::error_code &ec) {
  if (ctx_) {
    std::error_code t_ec;
    // NOLINTNEXTLINE(performance-no-int-to-ptr)
    ctx_->cancel((::native_handle)socket_, t_ec);
  }
  UdpSocket tcp(socket_);
  tcp.close(ec);
  socket_ = -1;
#ifdef __linux__
  ::close(write_socket_);
  write_socket_ = -1;
#endif
}

socket_type UdpSocketOp::native() const { return socket_; }
