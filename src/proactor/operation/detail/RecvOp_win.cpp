
#ifdef _WIN32

#include "proactor/operation/detail/RecvOp.h"

#include <winsock2.h>

#include "utils/error_code.h"

namespace detail {

RecvOp::RecvOp() {}

void RecvOp::async_recv(void *proactor, socket_type s, char *buff, size_t size,
                        func_type async_func, std::error_code &ec) {

  buff_ = {(uint32_t)size, (char *)buff};
  DWORD recv_flags = 0;
  func_ = async_func;
  if (::WSARecv(s, (WSABUF *)&buff_, 1, nullptr, &recv_flags,
                (LPWSAOVERLAPPED)this, nullptr)) {
    std::error_code re_ec = get_net_error_code();
    if (re_ec.value() != ERROR_IO_PENDING && re_ec.value() != 0) {
      ec = re_ec;
      complete(proactor, ec, 0);
      // assert(ec);
    }
  }
}

void RecvOp::complete(void *p, const std::error_code &result_ec,
                      size_t trans_size) {

  if (func_) {
    auto tmp = std::move(func_);
    tmp(p, result_ec, trans_size);
  }
}

} // namespace detail

#endif // _WIN32
