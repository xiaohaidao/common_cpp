
#ifdef _WIN32

#include "proactor/operation/detail/ReadOp.h"

#include <windows.h>

#include "utils/error_code.h"

namespace detail {

ReadOp::ReadOp() {}

void ReadOp::async_read(void *proactor, native_handle fd, const char *buff,
                        size_t size, func_type async_func,
                        std::error_code &ec) {

  func_ = async_func;
  // if (!::ReadFileEx(fd, (void *)buff, size, (LPWSAOVERLAPPED)this, nullptr))
  // {
  DWORD flag = 0;
  if (!::ReadFile(fd, (void *)buff, static_cast<DWORD>(size), &flag,
                  (LPWSAOVERLAPPED)this)) {
    std::error_code re_ec = getNetErrorCode();
    if (re_ec.value() != ERROR_IO_PENDING && re_ec.value() != 0) {
      ec = re_ec;
      complete(proactor, ec, 0);
      // assert(ec);
    }
  }
}

void ReadOp::complete(void *p, const std::error_code &result_ec,
                      size_t trans_size) {

  if (func_) {
    auto tmp = std::move(func_);
    tmp(p, result_ec, trans_size);
  }
}

} // namespace detail

#endif // _WIN32
