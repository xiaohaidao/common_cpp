
#ifdef _WIN32

#include "proactor/operation/detail/SendToOp.h"

#include <winsock2.h>

#include "utils/error_code.h"

namespace detail {

SendToOp::SendToOp() {}

void SendToOp::async_send_to(void *proactor, socket_type s, const char *buff,
                             size_t size, const SocketAddr &to,
                             func_type async_func, std::error_code &ec) {

  buff_ = {(uint32_t)size, (char *)buff};
  func_ = async_func;
  to_ = to;
  if (::WSASendTo(s, (WSABUF *)&buff_, 1, nullptr, 0,
                  (sockaddr *)to_.native_addr(),
                  static_cast<int>(to_.native_addr_size()),
                  (LPWSAOVERLAPPED)this, nullptr)) {
    std::error_code re_ec = getNetErrorCode();
    if (re_ec.value() != ERROR_IO_PENDING && re_ec.value() != 0) {
      ec = re_ec;
      complete(proactor, ec, 0);
      // assert(ec);
    }
  }
}

void SendToOp::complete(void *p, const std::error_code &result_ec,
                        size_t trans_size) {

  if (func_) {
    auto tmp = std::move(func_);
    tmp(p, result_ec, trans_size);
  }
}

} // namespace detail

#endif // _WIN32
