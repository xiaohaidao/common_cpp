
#include "proactor/operation/EventOp.h"

#include "proactor/Proactor.h"
#include "utils/error_code.h"

event_op::event_op() = default;

event_op::event_op(proactor *context) : ctx_(context) {
#ifdef __linux__
  std::error_code ec;
  op_ = detail::event_op::create(ec);
#endif
}

event_op::~event_op() {
#ifdef __linux__
  std::error_code ec;
  op_.close(ec);
#endif
}

event_op::event_op(const event_op &other) : ctx_(other.ctx_) {}

event_op &event_op::operator=(const event_op &other) {
  if (&other == this) {
    return *this;
  }
  this->ctx_ = other.ctx_;
  return *this;
}

#ifdef __linux__

void event_op::async_notify(const func_type &f, std::error_code &ec) {
  op_.async_wait(ctx_, f, ec);
  op_.notify(ec);
}

#else
void event_op::async_notify(const func_type &f, std::error_code &ec) {
  op_.async_notify(ctx_, f, ec);
}

void event_op::Event::async_notify(void *proactor, const func_type &async_func,
                                   std::error_code &ec) {
  func_ = async_func;
  if (!proactor) {
    complete(nullptr, {}, 0);
    return;
  }
  static_cast<proactor *>(proactor)->notify_op(this, ec);
}

void event_op::Event::complete(void *, const std::error_code &result_ec,
                               size_t trans_size) {

  if (func_) {
    auto tmp = std::move(func_);
    tmp(result_ec, trans_size);
  }
}

#endif
