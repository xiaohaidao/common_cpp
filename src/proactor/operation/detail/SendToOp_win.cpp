
#ifdef _WIN32

#include "SendToOp.h"

#include <winsock2.h>

#include "utils/error_code.h"

namespace detail {

SendToOp::SendToOp() {}

void SendToOp::async_send_to(sockets::socket_type s, const char *buff,
                             size_t size, func_type f,
                             const sockets::SocketAddr &to,
                             std::error_code &ec) {
  buff_ = {size, buff};
  func_ = async_func;
  to_ = to;
  if (!::WSASendTo(s, (WSABUF *)&buff_, 1, nullptr, nullptr, to_.native_addr(),
                   to_.native_addr_size(), this, nullptr)) {
    ec = getNetErrorCode();
  }
}

void SendToOp::complete(Proactor *p, const std::error_code &result_ec,
                        size_t trans_size) override {

  if (func_)
    func_(p, result_ec, trans_size);
}

} // namespace detail

#endif // _WIN32
