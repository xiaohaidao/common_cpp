
#include "proactor/Proactor.h"

#include "utils/error_code.h"

void Proactor::cancel_timeout(Operation *op, std::error_code &ec) {
  std::lock_guard<std::mutex> lck(timer_mutex_);
  timer_queue_.cancel(op);
}

void Proactor::post_timeout(Operation *op,
                            const typename time_clock::time_point &expire,
                            std::error_code &ec) {

  post_timeout(op, expire, time_clock::duration::zero(), ec);
}

void Proactor::post_timeout(Operation *op,
                            const typename time_clock::time_point &expire,
                            const typename time_clock::duration &interval,
                            std::error_code &ec) {

  {
    std::lock_guard<std::mutex> lck(timer_mutex_);
    timer_queue_.cancel(op);
    timer_queue_.push(expire, interval, op);
  }
  notify_op(nullptr, ec);
}