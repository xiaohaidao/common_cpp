
#ifdef __linux__

#include "proactor/operation/detail/WriteOp.h"

#include <unistd.h>

#include <utility>

#include "proactor/Proactor.h"
#include "utils/error_code.h"

namespace detail {

write_op::write_op() : buff_({}) {}

void write_op::async_write(void *proactor, func_type async_func,
                           native_handle fd, const char *buff, size_t size,
                           std::error_code &ec) {

  func_ = std::move(async_func);
  fd_ = fd;
  buff_ = {(uint32_t)size, (char *)buff};
  if (proactor == nullptr) {
    std::error_code re_ec = {ENXIO, std::system_category()};
    complete(proactor, re_ec, 0);
    return;
  }
  set_event_data(WRITE_OP_ENUM_ONCE);
  static_cast< ::proactor *>(proactor)->post(fd, this, ec);
}

void write_op::complete(void *p, const std::error_code &result_ec,
                        size_t /*trans_size*/) {

  std::error_code re_ec = result_ec;
  if (func_) {
    int re_size = 0;
    if (!re_ec) {
      re_size = ::write(fd_, buff_.buff, buff_.len);
      if (re_size < 0) {
        re_ec = get_error_code();
        re_size = 0;
      }
    }
    auto tmp = std::move(func_);
    tmp(p, re_ec, re_size);
  }
}

} // namespace detail

#endif // __linux__
