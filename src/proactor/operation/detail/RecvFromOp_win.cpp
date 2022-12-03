
#ifdef _WIN32

#include "operation/detail/RecvFromOp.h"

#include <winsock2.h>

#include "utils/error_code.h"

namespace detail {

RecvFromOp::RecvFromOp() {}

void RecvFromOp::async_read_from(sockets::socket_type s, char *buff,
                                 size_t size, func_type async_func,
                                 std::error_code &ec) {
  buff_ = {size, buff};
  func_ = async_func;
  if (!::WSARecvFrom(s, (WSABUF *)&buff_, 1, nullptr, nullptr,
                     from_.native_addr(), from_.native_addr_size(), this,
                     nullptr)) {
    ec = getNetErrorCode();
  }
}

void RecvFromOp::complete(Proactor *p, const std::error_code &result_ec,
                          size_t trans_size) {

  if (func_)
    func_(p, result_ec, trans_size, from_);
}

} // namespace detail

#endif // _WIN32
