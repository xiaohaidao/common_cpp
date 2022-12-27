
#ifdef _WIN32

#include "proactor/operation/detail/RecvOp.h"

#include <winsock2.h>

#include "utils/error_code.h"

namespace detail {

RecvOp::RecvOp() {}

void RecvOp::async_recv(sockets::socket_type s, char *buff, size_t size,
                        func_type async_func, std::error_code &ec) {

  buff_ = {(uint32_t)size, (char *)buff};
  DWORD recv_flags = 0;
  func_ = async_func;
  if (!::WSARecv(s, (WSABUF *)&buff_, 1, nullptr, &recv_flags,
                 (LPWSAOVERLAPPED)this, nullptr)) {
    ec = getNetErrorCode();
  }
}

void RecvOp::complete(void *p, const std::error_code &result_ec,
                      size_t trans_size) {

  if (func_)
    func_(p, result_ec, trans_size);
}

} // namespace detail

#endif // _WIN32
