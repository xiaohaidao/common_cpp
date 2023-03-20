
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

  void push(size_t timeout_us, Operation *op) { push(timeout_us, 0, op); }

  void push(size_t timeout_us, size_t interval_us, Operation *op) {
    timer_queue_t timeout;
    using namespace std::chrono;
    timeout.expire = clock_type::now() + microseconds(timeout_us);
    timeout.interval = microseconds(interval_us);
    timeout.op = op;
    heap_.push(timeout);
  }

  void cancel(Operation *op) { heap_.erase(op); }

  int wait_duration_usec(size_t max_us) {
    if (heap_.empty()) {
      return max_us;
    }
    using namespace std::chrono;
    int us =
        duration_cast<microseconds>(heap_.front()->expire - clock_type::now());
    return us > max_us ? max_us : us;
  }

  int wait_duration_ms(size_t max_ms) {
    if (heap_.empty()) {
      return max_ms;
    }
    using namespace std::chrono;
    int ms =
        duration_cast<milliseconds>(heap_.front()->expire - clock_type::now())
            .count();
    return ms > max_ms ? max_ms : ms;
  }

  void get_all_task(QueueOp &ops) {
    time_type now = clock_type::now();
    while (heap_.front()->expire <= now) {
      timer_queue_t timeout = *heap_.front();
      heap_.pop();
      ops.push(timeout.op);
      if (timeout.interval.time_since_epoch() > time_type::duration::zero()) {
        int p =
            (now - timeout.expire) / timeout.interval.time_since_epoch() + 1;
        printf("timer_queue_set get all task p value %d\n", p);
        timeout.expire += (timeout.interval.time_since_epoch() * p);
        heap_.push(timeout);
      }
    }
  }

private:
  struct timer_queue_t {
    timer_queue_t() : expire(0), interval(0), op(0) {}

    time_type expire;
    time_type interval;
    Operation *op;

    bool operator<(const timer_queue_t &other) const {
      return expire < other.expire;
    }
    bool operator<(const Operation *other_op) const { return op != other_op; }
  };

  min_heap<timer_queue_t> heap_;

}; // class TimerQueueSet

#endif // PROACTOR_OPERATION_DETAIL_TIMERQUEUESET_H
