
#ifdef _WIN32

#include "proactor/operation/detail/RecvFromOp.h"

#include <winsock2.h>

#include "utils/error_code.h"

namespace detail {

RecvFromOp::RecvFromOp() {}

void RecvFromOp::async_recv_from(void *proactor, socket_type s, char *buff,
                                 size_t size, func_type async_func,
                                 std::error_code &ec) {

  buff_ = {(uint32_t)size, buff};
  func_ = async_func;
  from_size_ = from_.native_addr_size();
  if (::WSARecvFrom(s, (WSABUF *)&buff_, 1, nullptr, nullptr,
                    (sockaddr *)from_.native_addr(), &from_size_,
                    (LPWSAOVERLAPPED)this, nullptr)) {
    std::error_code re_ec = getNetErrorCode();
    if (re_ec.value() != ERROR_IO_PENDING) {
      ec = re_ec;
      complete(proactor, ec, 0);
      // assert(ec);
    }
  }
}

void RecvFromOp::complete(void *p, const std::error_code &result_ec,
                          size_t trans_size) {

  if (func_)
    func_(p, result_ec, trans_size, from_);
}

} // namespace detail

#endif // _WIN32
