
#ifdef _WIN32

#include "proactor/operation/detail/ReadOp.h"

#include <windows.h>

#include "utils/error_code.h"

namespace detail {

ReadOp::ReadOp() {}

void ReadOp::async_read(native_handle fd, const char *buff, size_t size,
                        func_type async_func, std::error_code &ec) {

  func_ = async_func;
  if (!::ReadFileEx(fd, buff, size, (LPWSAOVERLAPPED)this, nullptr)) {
    ec = getNetErrorCode();
  }
}

void ReadOp::complete(void *p, const std::error_code &result_ec,
                      size_t trans_size) {

  if (func_)
    func_(p, result_ec, trans_size);
}

} // namespace detail

#endif // _WIN32