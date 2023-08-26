
#ifndef PROACTOR_OPERATION_TIMEROF_H
#define PROACTOR_OPERATION_TIMEROF_H

#include <functional>

#include "proactor/Proactor.h"

class TimerOp {
public:
  typedef std::function<void(const std::error_code &, size_t)> func_type;

  typedef typename Proactor::time_clock time_clock;

  explicit TimerOp(Proactor &context);

  void set_timeout(size_t expire_ms);
  void set_timeout(size_t expire_ms, size_t interval_ms);

  void wait();
  time_clock::duration expire();

  void async_wait(func_type async_func, std::error_code &ec);

  void close(std::error_code &ec);

private:
  Proactor *ctx_;

  struct TimerOpPrivate : public Operation {
    func_type func;
    time_clock::time_point expire;
    time_clock::duration interval;
    int64_t timeout_num;

    virtual void complete(void *proactor, const std::error_code &result_ec,
                          size_t trans_size) override;
  } op_;

}; // class TimerOp

#endif // PROACTOR_OPERATION_TIMEROF_H
