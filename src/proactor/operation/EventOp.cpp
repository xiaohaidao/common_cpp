
#include "proactor/operation/EventOp.h"

#include "proactor/Proactor.h"
#include "utils/error_code.h"

EventOp::EventOp() : ctx_(nullptr) {}

EventOp::EventOp(Proactor *context) : ctx_(context) {}

EventOp::EventOp(const EventOp &other) : ctx_(other.ctx_) {}

EventOp &EventOp::operator=(const EventOp &other) {
  if (&other == this) {
    return *this;
  }
  this->ctx_ = other.ctx_;
  return *this;
}

void EventOp::async_notify(const func_type &f, std::error_code &ec) {
  op_.async_notify(ctx_, f, ec);
}

void EventOp::Event::async_notify(void *proactor, const func_type &async_func,
                                  std::error_code &ec) {
  func_ = async_func;
  if (!proactor) {
    complete(nullptr, {}, 0);
    return;
  }
  static_cast<Proactor *>(proactor)->notify_op(this, ec);
}

void EventOp::Event::complete(void *, const std::error_code &result_ec,
                              size_t trans_size) {

  if (func_) {
    auto tmp = std::move(func_);
    tmp(result_ec, trans_size);
  }
}
