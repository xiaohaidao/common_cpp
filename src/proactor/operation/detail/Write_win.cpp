
#ifdef _WIN32

#include "proactor/operation/detail/WriteOp.h"

#include <windows.h>

#include "utils/error_code.h"

namespace detail {

WriteOp::WriteOp() {}

void WriteOp::async_write(native_handle fd, const char *buff, size_t size,
                          func_type async_func, std::error_code &ec) {

  func_ = async_func;
  if (!::WriteFileEx(fd, buff, size, (LPWSAOVERLAPPED)this, nullptr)) {
    ec = getNetErrorCode();
  }
}

void WriteOp::complete(void *p, const std::error_code &result_ec,
                      size_t trans_size) {

  if (func_)
    func_(p, result_ec, trans_size);
}

} // namespace detail

#endif // _WIN32
