
#include "proactor/operation/TimerOp.h"

#include <thread>

using namespace std::chrono;

TimerOp::TimerOp(Proactor &context) : ctx_(&context) {}

void TimerOp::set_timeout(size_t expire_ms) { set_timeout(expire_ms, 0); }

void TimerOp::set_timeout(size_t expire_ms, size_t interval_ms) {
  op_.expire = time_clock::now() + milliseconds(expire_ms);
  op_.interval = milliseconds(interval_ms);
}

void TimerOp::wait(std::error_code &ec) {
  auto now = time_clock::now();
  if (op_.expire < now) {
    if (!(op_.interval > time_clock::duration::zero())) {
      return;
    }
    int64_t p = (now - op_.expire) / op_.interval + 1;
    op_.expire += (op_.interval * p);
  }
  std::this_thread::sleep_until(op_.expire);
}

TimerOp::time_clock::duration TimerOp::expire_us(std::error_code &ec) {
  return op_.expire - time_clock::now();
}

void TimerOp::async_wait(func_type async_func, std::error_code &ec) {
  if (!ctx_) {
    wait(ec);
    return;
  }
  op_.func = async_func;
  ctx_->post_timeout(&op_, op_.expire, op_.interval, ec);
}

void TimerOp::close(std::error_code &ec) {
  if (ctx_) {
    ctx_->cancel_timeout(&op_, ec);
  }
}

void TimerOp::TimerOpPrivate::complete(void *proactor,
                                       const std::error_code &result_ec,
                                       size_t trans_size) {
  func(result_ec, trans_size);
}
