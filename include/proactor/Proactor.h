
#ifndef PROACTOR_PROACTOR_H
#define PROACTOR_PROACTOR_H

#include "proactor/operation/detail/Operation.h"

#include <mutex>

#include "proactor/operation/detail/TimerQueueSet.h"

#ifndef _WIN32
#include "proactor/operation/detail/EventOp_posix.h"
#endif // _WIN32

struct ThreadInfo;

class proactor {
public:
  using time_clock = std::chrono::steady_clock;

  proactor(const proactor &) = delete;
  const proactor &operator=(const proactor &) = delete;

  explicit proactor(std::error_code &ec);

  void shutdown();

  size_t run();
  size_t run_one(size_t timeout_us, std::error_code &ec);

  void notify_op(operation *op, std::error_code &ec);

  void post(native_handle file_descriptor, operation *op, std::error_code &ec);

  void cancel(native_handle file_descriptor, std::error_code &ec);

  void close(std::error_code &ec);

  // timeout operation
  void cancel_timeout(operation *op, std::error_code &ec);
  void post_timeout(operation *op,
                    const typename time_clock::time_point &expire,
                    std::error_code &ec);
  void post_timeout(operation *op,
                    const typename time_clock::time_point &expire,
                    const typename time_clock::duration &interval,
                    std::error_code &ec);

private:
  proactor();

  size_t call_one(size_t timeout_us, ThreadInfo &thread_info,
                  std::error_code &ec);

  native_handle fd_{0};
  bool shutdown_{false};

  timer_queue_set<time_clock> timer_queue_;
  std::mutex timer_mutex_;

#ifdef __linux__
  ::detail::event_op event_;
  queue_op event_queue_;
  std::mutex event_mutex_;
#endif // __linux__
};

#endif // PROACTOR_PROACTOR_H
