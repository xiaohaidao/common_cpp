
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
    : fd_(Reactor(ec).native_handle()), shutdown_(false) {}

void Proactor::shutdown() {
  shutdown_ = true;

  std::error_code ec;
  notify_op(nullptr, ec); // TODO: multiple thread close
}

size_t Proactor::run() {
  std::error_code ec;
  size_t n = 0;
  ThreadInfo tread_info;
  while (call_one((std::numeric_limits<size_t>::max)(), tread_info, ec)) {
    if (n <= std::numeric_limits<size_t>::max()) {
      ++n;
    }
  }
  return n;
}

size_t Proactor::run_one(size_t timeout_us, std::error_code &ec) {
  ThreadInfo tread_info;
  return call_one(timeout_us, tread_info, ec);
}

void Proactor::notify_op(Operation *op, std::error_code &ec) {
  // TODO
  // queue.push(op);
  // if (!PostQueuedCompletionStatus(fd_, 0, 0, op)) {
  //   ec = getErrorCode();
  // }
}

void Proactor::post(const native_handle &file_descriptor, Operation *op,
                    std::error_code &ec) {

  Reactor reactor(fd_);
  reactor.post(file_descriptor, op, ec);
}

void Proactor::cancel(const native_handle &file_descriptor,
                      std::error_code &ec) {

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
    if (reactor.call_one(queue)) {
      return 1;
    }
    if (reactor.run_once_timeout(queue, timeout_us / 1000, ec) == 0) {
      return 0;
    }
  }
  return 0;
}

#endif // __linux__
