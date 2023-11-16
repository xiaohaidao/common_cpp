
#ifdef __linux__

#include "proactor/operation/detail/EventOp_posix.h"

#include <sys/eventfd.h>
#include <unistd.h>

#include "proactor/Proactor.h"
#include "utils/error_code.h"

namespace detail {

EventOp::EventOp() : fd_(-1) {}

void EventOp::close(std::error_code &ec) {
  if (::close(fd_) < 0) {
    ec = getErrorCode();
  }
}

native_handle EventOp::native_handle() const { return fd_; }

EventOp EventOp::create(std::error_code &ec) {
  EventOp re;
  re.fd_ = ::eventfd(0, 0);
  if (re.fd_ < 0) {
    ec = getErrorCode();
  }
  return re;
}

void EventOp::notify(std::error_code &ec) {
  uint64_t exp = 1;
  if (::write(fd_, &exp, sizeof(exp)) != sizeof(exp)) {
    ec = getErrorCode();
  }
}

uint64_t EventOp::wait(std::error_code &ec) {
  uint64_t exp = 0;
  if (::read(fd_, &exp, sizeof(exp)) != sizeof(exp)) {
    ec = getErrorCode();
  }
  return exp;
}

void EventOp::async_wait(void *proactor, func_type async_func,
                         std::error_code &ec) {

  func_ = async_func;
  if (proactor == nullptr) {
    std::error_code re_ec = {ENXIO, std::system_category()};
    complete(proactor, re_ec, 0);
    return;
  }
  set_event_data(READ_OP_ENUM_ONCE);
  static_cast<Proactor *>(proactor)->post(fd_, this, ec);
}

void EventOp::complete(void *p, const std::error_code &result_ec,
                       size_t trans_size) {

  std::error_code re_ec = result_ec;
  if (func_) {
    uint64_t count = wait(re_ec);
    auto tmp = std::move(func_);
    tmp(re_ec, count);
  }
}

} // namespace detail

#endif // __linux__
