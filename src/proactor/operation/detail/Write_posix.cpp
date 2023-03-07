
#ifdef __linux__

#include "proactor/operation/detail/WriteOp.h"

#include <unistd.h>

#include "utils/error_code.h"

namespace detail {

WriteOp::WriteOp() : fd_(0), buff_({}) {}

void WriteOp::async_write(native_handle fd, const char *buff, size_t size,
                        func_type async_func, std::error_code &ec) {

  func_ = async_func;
  fd_ = fd;
  buff_ = {(uint32_t)size, (char *)buff};
}

void WriteOp::complete(void *p, const std::error_code &result_ec,
                      size_t trans_size) {

  std::error_code re_ec = result_ec;
  if (func_) {
    int re_size = -1;
    if (!re_ec) {
      re_size = ::write(fd_, buff_.buff, buff_.len);
      if (re_size < 0) {
        re_ec = getErrorCode();
      }
    }
    func_(p, re_ec, re_size);
  }
}

} // namespace detail

#endif // __linux__
