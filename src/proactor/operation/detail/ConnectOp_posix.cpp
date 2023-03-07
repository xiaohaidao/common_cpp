
#ifdef __linux__

#include "proactor/operation/detail/ConnectOp.h"

#include <fcntl.h>
#include <sys/socket.h>

#include "utils/error_code.h"

namespace detail {

ConnectOp::ConnectOp() : client_(-1) {}

void ConnectOp::async_connect(socket_type s, const SocketAddr &addr,
                              func_type async_func, std::error_code &ec) {

  client_ = s;
  func_ = async_func;

  int flags = fcntl(client_, F_GETFL, 0);
  if (fcntl(client_, F_SETFL, flags | O_NONBLOCK)) {
    ec = getErrorCode();
    return;
  }
  if (::connect(client_, (const sockaddr *)addr.native_addr(),
                (int)addr.native_addr_size())) {
    std::error_code re_ec = getNetErrorCode();
    if (re_ec.value() != EINPROGRESS) {
      ec = re_ec;
    }
    return;
  }
}

void ConnectOp::complete(void *p, const std::error_code &result_ec,
                         size_t trans_size) {

  std::error_code ec;
  int flags = fcntl(client_, F_GETFL, 0);
  if (fcntl(client_, F_SETFL, flags & ~O_NONBLOCK)) {
    ec = getErrorCode();
  }
  std::error_code re_ec = result_ec;
  if (func_) {
    socket_type s = client_;
    client_ = -1;

    func_(p, re_ec, trans_size, s);
  }
}

} // namespace detail

#endif // __linux__
