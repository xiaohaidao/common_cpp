
#ifdef __linux__

#include "proactor/Proactor.h"

#include <limits>

#include "reactor/Reactor.h"
#include "utils/error_code.h"

struct ThreadInfo {
  QueueOp queue;
};

Proactor::Proactor() : fd_(0), shutdown_(false) {}

Proactor::Proactor(std::error_code &ec)
    : fd_(Reactor(ec).native_handle()), shutdown_(false),
      event_(::detail::EventOp::create(ec)) {}

void Proactor::shutdown() {
  shutdown_ = true;

  std::error_code ec;
  notify_op(nullptr, ec); // TODO: multiple thread close
}

size_t Proactor::run() {
  std::error_code ec;
  size_t n = 0;
  ThreadInfo tread_info;
  while (!shutdown_) {
    if (call_one((std::numeric_limits<size_t>::max)(), tread_info, ec) &&
        n <= std::numeric_limits<size_t>::max()) {

      ++n;
    }
  }
  shutdown();
  return n;
}

size_t Proactor::run_one(size_t timeout_us, std::error_code &ec) {
  ThreadInfo thread_info;
  size_t n = call_one(timeout_us, thread_info, ec);
  while (thread_info.queue.begin()) {
    if (call_one(timeout_us, thread_info, ec)) {
      ++n;
    }
  }
  return n;
}

void Proactor::notify_op(Operation *op, std::error_code &ec) {
  if (op) {
    std::lock_guard<std::mutex> lck(event_mutex_);
    event_queue_.push(op);
  }
  auto call = [](const std::error_code &re_ec, size_t re_size) {};
  event_.async_wait(this, call, ec);
  event_.notify(ec);
}

void Proactor::post(native_handle file_descriptor, Operation *op,
                    std::error_code &ec) {

  Reactor reactor(fd_);
  reactor.post(file_descriptor, op, ec);
}

void Proactor::cancel(native_handle file_descriptor, std::error_code &ec) {

  Reactor reactor(fd_);
  reactor.cancel(file_descriptor, ec);
}

void Proactor::close(std::error_code &ec) {
  Reactor reactor(fd_);
  reactor.close(ec);
}

size_t Proactor::call_one(size_t timeout_us, ThreadInfo &thread_info,
                          std::error_code &ec) {

  Reactor reactor(fd_);
  QueueOp &queue = thread_info.queue;
  for (; !shutdown_;) {
    if (!queue.begin()) {
      int timeout_ms = timer_queue_.wait_duration_ms(timeout_us / 1000);
      if (timeout_ms <= 0) {
        std::lock_guard<std::mutex> lck(timer_mutex_);
        timer_queue_.get_all_task(queue);
      } else {
        if (reactor.run_once_timeout(queue, timeout_ms, ec) == 0) {
          return 0;
        }
      }
    }
    if (queue.begin() == &event_) {
      reactor.call_one(queue); // pop event
      std::lock_guard<std::mutex> lck(event_mutex_);
      queue.push(event_queue_);
    }
    if (reactor.call_one(queue)) {
      return 1;
    }
  }
  return 0;
}

#endif // __linux__
