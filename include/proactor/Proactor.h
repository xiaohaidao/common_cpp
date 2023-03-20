
#ifndef PROACTOR_PROACTOR_H
#define PROACTOR_PROACTOR_H

#include "proactor/operation/detail/Operation.h"

#include <mutex>

#include "proactor/operation/detail/TimerQueueSet.h"

#ifndef _WIN32
#include "proactor/operation/detail/EventOp_posix.h"
#endif // _WIN32

class ThreadInfo;

class Proactor {
public:
  Proactor(const Proactor &) = delete;
  const Proactor &operator=(const Proactor &) = delete;

  explicit Proactor(std::error_code &ec);

  void shutdown();

  size_t run();
  size_t run_one(size_t timeout_us, std::error_code &ec);

  void stop(std::error_code &ec);

  void notify_op(Operation *op, std::error_code &ec);

  void post(native_handle file_descriptor, Operation *op, std::error_code &ec);
  void cancel(native_handle file_descriptor, std::error_code &ec);

  void close(std::error_code &ec);

private:
  Proactor();

  size_t call_one(size_t timeout_us, ThreadInfo &thread_info,
                  std::error_code &ec);

  native_handle fd_;
  bool shutdown_;

  TimerQueueSet<std::chrono::steady_clock> timer_queue_;
  std::mutex timer_mutex_;

#ifndef _WIN32
  ::detail::EventOp event_;
  QueueOp event_queue_;
  std::mutex event_mutex_;
#endif // _WIN32
};

#endif // PROACTOR_PROACTOR_H
