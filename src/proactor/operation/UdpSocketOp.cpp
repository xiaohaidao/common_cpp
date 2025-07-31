
#include "proactor/operation/UdpSocketOp.h"

#ifdef __linux__
#include <unistd.h>
#endif

#include "sockets/UdpSocket.h"
#include "utils/error_code.h"

udp_socket_op::udp_socket_op(proactor &context) : ctx_(&context) {}

udp_socket_op::udp_socket_op(proactor &context, socket_type s)
    : ctx_(&context), socket_(s) {

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

udp_socket_op::udp_socket_op(const udp_socket_op &other)
    : ctx_(other.ctx_), socket_(other.socket_)
#ifdef __linux__
      ,
      write_socket_(other.write_socket_)
#endif
{
}

udp_socket_op &udp_socket_op::operator=(const udp_socket_op &other) {
  if (&other == this) {
    return *this;
  }
  this->ctx_ = other.ctx_;
  this->socket_ = other.socket_;
#ifdef __linux__
  this->write_socket_ = other.write_socket_;
#endif
  // this->recvfrom_op_ = detail::recv_from_op();
  // this->sendto_op_ = detail::send_to_op();
  return *this;
}

std::pair<size_t, socket_addr>
udp_socket_op::recv_from(char *buff, size_t buff_size, std::error_code &ec) {

  udp_socket tcp(socket_);
  return tcp.recv_from(buff, buff_size, ec);
}

size_t udp_socket_op::send_to(const char *buff, size_t buff_size,
                              const socket_addr &to, std::error_code &ec) {

  udp_socket tcp(socket_);
  return tcp.send_to(buff, buff_size, to, ec);
}

void udp_socket_op::async_read(char *buff, size_t buff_size,
                               const udp_socket_op::func_recv_type &f,
                               std::error_code &ec) {

  auto call_back = [f](void * /*ctx*/, const std::error_code &re_ec,
                       size_t recv_size,
                       const socket_addr &from) { f(re_ec, recv_size, from); };

  recvfrom_op_.async_recv_from(ctx_, socket_, buff, buff_size, call_back, ec);
}

void udp_socket_op::async_write(const char *buff, size_t buff_size,
                                const socket_addr &to,
                                const udp_socket_op::func_send_type &f,
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

void udp_socket_op::create(FamilyType family, std::error_code &ec) {
  if (socket_ > 0) {
    std::error_code t_ec;
    close(t_ec);
  }
  udp_socket const udp = udp_socket::create(family, ec);
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

void udp_socket_op::bind(const char *port_or_service, std::error_code &ec) {
  bind(port_or_service, kIpV4, ec);
}

void udp_socket_op::bind(const char *port_or_service, FamilyType family,
                         std::error_code &ec) {
  if (socket_ > 0) {
    std::error_code t_ec;
    close(t_ec);
  }
  udp_socket const udp = udp_socket::bind(port_or_service, family, ec);
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

void udp_socket_op::close(std::error_code &ec) {
  if (ctx_) {
    std::error_code t_ec;
    // NOLINTNEXTLINE(performance-no-int-to-ptr)
    ctx_->cancel((::native_handle)socket_, t_ec);
  }
  udp_socket tcp(socket_);
  tcp.close(ec);
  socket_ = -1;
#ifdef __linux__
  ::close(write_socket_);
  write_socket_ = -1;
#endif
  recvfrom_op_ = {};
  sendto_op_ = {};
}

socket_type udp_socket_op::native() const { return socket_; }
