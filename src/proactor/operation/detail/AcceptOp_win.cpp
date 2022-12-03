
#ifdef _WIN32

#include "operation/detail/AcceptOp.h"

#include <mswsock.h>
#include <winsock2.h>

#include "utils/error_code.h"

namespace detail {

AcceptOp::AcceptOp() {}

void AcceptOp::async_accept(sockets::socket_type s, func_type async_func,
                            std::error_code &ec) {
  if (clinet_ == INVALID_SOCKET) {
    clinet_ = sockets::socket(kUnspecified, kStream, kTCP);
  }
  func_ = async_func;
  if (!::AcceptEx(s, clinet_, addresses_, 0, 32, 32, nullptr, &accpet_impl_)) {
    ec = getNetErrorCode();
  }
}

void AcceptOp::AcceptImplOp::complete(Proactor *p,
                                      const std::error_code &result_ec,
                                      size_t trans_size) {

  if (func_) {
    std::pair<sockets::socket_type, sockets::SocketAddr> ac_addr;
    ac_addr.first = clinet_;
    ac_addr.second = sockets::SocketAddr((sockaddr *)(addresses_ + 32));
    clinet_ = INVALID_SOCKET;
    memset(addresses_, 0, 64);
    func_(p, result_ec, std::move(ac_addr));
  }
}

} // namespace detail

#endif // _WIN32
