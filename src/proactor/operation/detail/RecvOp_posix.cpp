
#ifdef __linux__

#include "proactor/operation/detail/RecvOp.h"

#include <sys/socket.h>
#include <unistd.h>

#include "proactor/Proactor.h"
#include "utils/error_code.h"

namespace detail {

RecvOp::RecvOp() : socket_(-1) {}

void RecvOp::async_recv(sockets::socket_type s, char *buff, size_t size,
                        func_type async_func, std::error_code &ec) {

  buff_ = {(uint32_t)size, (char *)buff};
  func_ = async_func;
  socket_ = s;
}

void RecvOp::complete(void *p, const std::error_code &result_ec,
                      size_t trans_size) {

  if (p) {
    std::error_code ec;
    static_cast<Proactor *>(p)->cancel(socket_, ec);
  }
  std::error_code re_ec = result_ec;
  if (func_) {
    int re_size = -1;
    if (!re_ec) {
      re_size = ::recv(socket_, buff_.buff, buff_.len, 0);
      if (re_size < 0) {
        re_ec = getErrorCode();
      }
    }
    func_(p, re_ec, re_size);
  }
}

} // namespace detail

#endif // __linux__
