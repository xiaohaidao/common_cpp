
#ifdef __linux__

#include "proactor/operation/detail/SendToOp.h"

#include <sys/socket.h>
#include <unistd.h>

#include "utils/error_code.h"

namespace detail {

SendToOp::SendToOp() : socket_(-1) {}

void SendToOp::async_send_to(socket_type s, const char *buff, size_t size,
                             func_type async_func, const SocketAddr &to,
                             std::error_code &ec) {

  buff_ = {(uint32_t)size, (char *)buff};
  func_ = async_func;
  to_ = to;
  socket_ = s;
  // if (!::WSASendTo(s, (WSABUF *)&b, 1, nullptr, 0,
  //                  (sockaddr *)to_.native_addr(), to_.native_addr_size(),
  //                  (LPWSAOVERLAPPED)this, nullptr)) {
  //   ec = getNetErrorCode();
  // }
}

void SendToOp::complete(void *p, const std::error_code &result_ec,
                        size_t trans_size) {

  std::error_code re_ec = result_ec;
  if (func_)
    func_(p, result_ec, trans_size);
}

} // namespace detail

#endif // __linux__
