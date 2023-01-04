
#ifdef _WIN32

#include "proactor/operation/detail/RecvFromOp.h"

#include <winsock2.h>

#include "utils/error_code.h"

namespace detail {

RecvFromOp::RecvFromOp() {}

void RecvFromOp::async_recv_from(socket_type s, char *buff, size_t size,
                                 func_type async_func, std::error_code &ec) {

  WSABUF b = {(uint32_t)size, buff};
  func_ = async_func;
  from_size_ = 0;
  if (!::WSARecvFrom(s, (WSABUF *)&b, 1, nullptr, nullptr,
                     (sockaddr *)from_.native_addr(), &from_size_,
                     (LPWSAOVERLAPPED)this, nullptr)) {
    ec = getNetErrorCode();
  }
}

void RecvFromOp::complete(void *p, const std::error_code &result_ec,
                          size_t trans_size) {

  if (func_)
    func_(p, result_ec, trans_size, from_);
}

} // namespace detail

#endif // _WIN32
