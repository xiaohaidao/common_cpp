
#include "proactor/Proactor.h"

#include "utils/error_code.h"

void proactor::cancel_timeout(operation *op, std::error_code & /*ec*/) {
  std::lock_guard<std::mutex> const lck(timer_mutex_);
  timer_queue_.cancel(op);
}

void proactor::post_timeout(operation *op,
                            const typename time_clock::time_point &expire,
                            std::error_code &ec) {

  post_timeout(op, expire, time_clock::duration::zero(), ec);
}

void proactor::post_timeout(operation *op,
                            const typename time_clock::time_point &expire,
                            const typename time_clock::duration &interval,
                            std::error_code &ec) {

  {
    std::lock_guard<std::mutex> const lck(timer_mutex_);
    timer_queue_.cancel(op);
    timer_queue_.push(expire, interval, op);
  }
  notify_op(nullptr, ec);
}
