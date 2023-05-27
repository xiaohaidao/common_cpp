
#ifndef PROACTOR_OPERATION_DETAIL_TIMERQUEUESET_H
#define PROACTOR_OPERATION_DETAIL_TIMERQUEUESET_H

#include <chrono>
#include <functional>

#include "min_heap/min_heap.h"
#include "reactor/detail/QueueOp.h"

template <typename Clock> class TimerQueueSet {
public:
  typedef typename Clock::time_point time_type;
  typedef typename time_type::clock clock_type;

  TimerQueueSet() {}

  void push_ms(size_t timeout_ms, Operation *op) {
    push_us(timeout_ms * 1000, 0, op);
  }
  void push_ms(size_t timeout_ms, size_t interval_ms, Operation *op) {
    push_us(timeout_ms * 1000, interval_ms * 1000, op);
  }

  void push_us(size_t timeout_us, Operation *op) { push_us(timeout_us, 0, op); }
  void push_us(size_t timeout_us, size_t interval_us, Operation *op) {
    using namespace std::chrono;
    push(clock_type::now() + microseconds(timeout_us),
         microseconds(interval_us));
  }

  void push(const time_type &timeout,
            const typename time_type::duration &interval, Operation *op) {

    timer_queue_t queue_op;
    using namespace std::chrono;
    queue_op.expire = timeout;
    queue_op.interval = interval;
    queue_op.op = op;
    heap_.push(queue_op);
  }

  void cancel(Operation *op) { heap_.erase(op); }

  int wait_duration_usec(size_t max_us) {
    if (heap_.empty()) {
      return max_us;
    }
    using namespace std::chrono;
    auto diff = heap_.front()->expire - clock_type::now();
    return (microseconds(max_us) < diff)
               ? static_cast<int>(max_us)
               : duration_cast<microseconds>(diff).count();
  }

  int wait_duration_ms(size_t max_ms) {
    if (heap_.empty()) {
      return static_cast<int>(max_ms);
    }
    using namespace std::chrono;
    auto diff = heap_.front()->expire - clock_type::now();
    return (milliseconds(max_ms) < diff)
               ? static_cast<int>(max_ms)
               : static_cast<int>(duration_cast<milliseconds>(diff).count());
  }

  void get_all_task(QueueOp &ops) {
    time_type now = clock_type::now();
    while (!heap_.empty() && heap_.front()->expire <= now) {
      timer_queue_t timeout = *heap_.front();
      heap_.pop();
      ops.push(timeout.op);
      if (timeout.interval > time_type::duration::zero()) {
        int64_t p = (now - timeout.expire) / timeout.interval + 1;
        timeout.expire += (timeout.interval * p);
        heap_.push(timeout);
      }
    }
  }

private:
  struct timer_queue_t {
    timer_queue_t() : op(0) {}

    time_type expire;
    typename time_type::duration interval;
    Operation *op;

    bool operator<(const timer_queue_t &other) const {
      return expire < other.expire;
    }
    // bool operator<(const Operation *other_op) const { return op != other_op;
    // } bool operator==(const timer_queue_t &other) const { return op ==
    // other.op; }
    bool operator==(const Operation *other_op) const { return op == other_op; }
  };

  min_heap<timer_queue_t> heap_;

}; // class TimerQueueSet

#endif // PROACTOR_OPERATION_DETAIL_TIMERQUEUESET_H
