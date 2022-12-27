
#include "proactor/operation/TcpStreamOp.h"

#include "sockets/TcpStream.h"
#include "utils/error_code.h"

using sockets::TcpStream;

TcpStreamOp::TcpStreamOp(const TcpStreamOp &other)
    : ctx_(other.ctx_), socket_(other.socket_) {}

const TcpStreamOp &TcpStreamOp::operator=(const TcpStreamOp &other) {
  this->ctx_ = other.ctx_;
  this->socket_ = other.socket_;
  this->connect_op_ = detail::ConnectOp();
  this->recv_op_ = detail::RecvOp();
  this->send_op_ = detail::SendOp();
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

sockets::socket_type TcpStreamOp::native_handle() const { return socket_; }
