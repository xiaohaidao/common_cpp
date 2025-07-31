
#ifdef __linux__

#include "proactor/operation/detail/EventOp_posix.h"

#include <sys/eventfd.h>
#include <unistd.h>

#include <utility>

#include "proactor/Proactor.h"
#include "utils/error_code.h"

namespace detail {

event_op::event_op() = default;

void event_op::close(std::error_code &ec) {
  if (::close(fd_) < 0) {
    ec = get_error_code();
  }
}

native_handle event_op::native() const { return fd_; }

event_op event_op::create(std::error_code &ec) {
  event_op re;
  re.fd_ = ::eventfd(0, 0);
  if (re.fd_ < 0) {
    ec = get_error_code();
  }
  return re;
}

void event_op::notify(std::error_code &ec) {
  uint64_t exp = 1;
  if (::write(fd_, &exp, sizeof(exp)) != sizeof(exp)) {
    ec = get_error_code();
  }
}

uint64_t event_op::wait(std::error_code &ec) {
  uint64_t exp = 0;
  if (::read(fd_, &exp, sizeof(exp)) != sizeof(exp)) {
    ec = get_error_code();
  }
  return exp;
}

void event_op::async_wait(void *proactor, event_op::func_type async_func,
                          std::error_code &ec) {

  func_ = std::move(async_func);
  if (proactor == nullptr) {
    std::error_code re_ec = {ENXIO, std::system_category()};
    complete(proactor, re_ec, 0);
    return;
  }
  set_event_data(READ_OP_ENUM_ONCE);
  static_cast< ::proactor *>(proactor)->post(fd_, this, ec);
}

void event_op::complete(void * /*p*/, const std::error_code &result_ec,
                        size_t /*trans_size*/) {

  std::error_code re_ec = result_ec;
  if (func_) {
    uint64_t count = wait(re_ec);
    auto tmp = std::move(func_);
    tmp(re_ec, count);
  }
}

} // namespace detail

#endif // __linux__
