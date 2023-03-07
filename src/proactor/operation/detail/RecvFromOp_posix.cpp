
#ifdef __linux__

#include "proactor/operation/detail/RecvFromOp.h"

#include <sys/socket.h>
#include <unistd.h>

#include "utils/error_code.h"

namespace detail {

RecvFromOp::RecvFromOp() {}

void RecvFromOp::async_recv_from(socket_type s, char *buff, size_t size,
                                 func_type async_func, std::error_code &ec) {

  buff_ = {(uint32_t)size, (char *)buff};
  func_ = async_func;
  from_size_ = 0;
}

void RecvFromOp::complete(void *p, const std::error_code &result_ec,
                          size_t trans_size) {

  if (func_)
    func_(p, result_ec, trans_size, from_);
}

} // namespace detail

#endif // __linux__
