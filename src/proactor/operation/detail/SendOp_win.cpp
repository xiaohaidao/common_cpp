
#ifdef _WIN32

#include "proactor/operation/detail/SendOp.h"

#include <winsock2.h>

#include "utils/error_code.h"

namespace detail {

SendOp::SendOp() {}

void SendOp::async_send(void *proactor, socket_type s, const char *buff,
                        size_t size, func_type async_func,
                        std::error_code &ec) {

  buff_ = {(uint32_t)size, (char *)buff};
  func_ = async_func;
  if (::WSASend(s, (WSABUF *)&buff_, 1, nullptr, 0, (LPWSAOVERLAPPED)this,
                nullptr)) {
    std::error_code re_ec = get_net_error_code();
    if (re_ec.value() != ERROR_IO_PENDING && re_ec.value() != 0) {
      ec = re_ec;
      complete(proactor, ec, 0);
      // assert(ec);
    }
  }
}

void SendOp::complete(void *p, const std::error_code &result_ec,
                      size_t trans_size) {

  if (func_) {
    auto tmp = std::move(func_);
    tmp(p, result_ec, trans_size);
  }
}

} // namespace detail

#endif // _WIN32
