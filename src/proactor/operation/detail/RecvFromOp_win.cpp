
#ifdef _WIN32

#include "proactor/operation/detail/RecvFromOp.h"

#include <winsock2.h>

#include <utility>

#include "utils/error_code.h"

namespace detail {

RecvFromOp::RecvFromOp() {}

void RecvFromOp::async_recv_from(void *proactor, socket_type s, char *buff,
                                 size_t size, func_type async_func,
                                 std::error_code &ec) {

  buff_ = {(uint32_t)size, buff};
  func_ = std::move(async_func);
  from_size_ = static_cast<int>(from_.native_addr_size());
  DWORD recv_flags = 0;
  if (::WSARecvFrom(s, (WSABUF *)&buff_, 1, nullptr, &recv_flags,
                    (sockaddr *)from_.native_addr(), &from_size_,
                    (LPWSAOVERLAPPED)this, nullptr)) {
    std::error_code const re_ec = get_net_error_code();
    if (re_ec.value() != ERROR_IO_PENDING && re_ec.value() != 0) {
      ec = re_ec;
      complete(proactor, ec, 0);
      // assert(ec);
    }
  }
}

void RecvFromOp::complete(void *p, const std::error_code &result_ec,
                          size_t trans_size) {

  if (func_) {
    auto tmp = std::move(func_);
    tmp(p, result_ec, trans_size, from_);
  }
}

} // namespace detail

#endif // _WIN32
