
#ifdef _WIN32

#include "proactor/operation/detail/WriteOp.h"

#include <windows.h>

#include "utils/error_code.h"

namespace detail {

WriteOp::WriteOp() {}

void WriteOp::async_write(void *proactor, native_handle fd, const char *buff,
                          size_t size, func_type async_func,
                          std::error_code &ec) {

  func_ = async_func;
  // if (!::WriteFileEx(fd, buff, size, (LPWSAOVERLAPPED)this, nullptr)) {
  DWORD flag = 0;
  if (!::WriteFile(fd, buff, static_cast<DWORD>(size), &flag,
                   (LPWSAOVERLAPPED)this)) {
    std::error_code re_ec = getNetErrorCode();
    if (re_ec.value() != ERROR_IO_PENDING && re_ec.value() != 0) {
      ec = re_ec;
      complete(proactor, ec, 0);
      // assert(ec);
    }
  }
}

void WriteOp::complete(void *p, const std::error_code &result_ec,
                       size_t trans_size) {

  if (func_) {
    auto tmp = std::move(func_);
    tmp(p, result_ec, trans_size);
  }
}

} // namespace detail

#endif // _WIN32
