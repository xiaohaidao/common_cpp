
#ifdef __linux__

#include <utility>

#include "proactor/operation/detail/SendOp.h"

#include "proactor/Proactor.h"
#include "sockets/TcpStream.h"
#include "utils/error_code.h"

namespace detail {

SendOp::SendOp() : socket_(-1) {}

void SendOp::async_send(void *proactor, socket_type s, const char *buff,
                        size_t size, func_type async_func,
                        std::error_code &ec) {

  buff_ = {(uint32_t)size, (char *)buff};
  func_ = std::move(async_func);
  socket_ = s;
  if (proactor == nullptr) {
    std::error_code re_ec = {ENXIO, std::system_category()};
    complete(proactor, re_ec, 0);
    return;
  }
  set_event_data(WRITE_OP_ENUM_ONCE);
  static_cast<Proactor *>(proactor)->post(s, this, ec);
}

void SendOp::complete(void *p, const std::error_code &result_ec,
                      size_t /*trans_size*/) {

  std::error_code re_ec = result_ec;
  if (func_) {
    size_t re_size = 0;
    if (!re_ec) {
      TcpStream tcp(socket_);
      re_size = tcp.write(buff_.buff, buff_.len, re_ec);
    }
    auto tmp = std::move(func_);
    tmp(p, re_ec, re_size);
  }
}

} // namespace detail

#endif // __linux__
