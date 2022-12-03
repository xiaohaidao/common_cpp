
#ifdef _WIN32

#include "operation/detail/RecvOp.h"

#include <winsock2.h>

#include "utils/error_code.h"

namespace detail {

RecvOp::RecvOp() {}

void RecvOp::async_read(sockets::socket_type s, char *buff, size_t size,
                        func_type async_func, std::error_code &ec) {

  buff_ = {size, buff};
  func_ = async_func;
  if (!::WSARecv(s, (WSABUF *)&buff_, 1, nullptr, nullptr, this, nullptr)) {
    ec = getNetErrorCode();
  }
}

void RecvOp::complete(Proactor *p, const std::error_code &result_ec,
                      size_t trans_size) {

  if (func_)
    func_(p, result_ec, trans_size);
}

} // namespace detail

#endif // _WIN32
