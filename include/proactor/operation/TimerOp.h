
#ifndef PROACTOR_OPERATION_TIMEROP_H
#define PROACTOR_OPERATION_TIMEROP_H

#include <functional>

#include "proactor/Proactor.h"

class timer_op {
public:
  using func_type = std::function<void(const std::error_code &, int64_t)>;

  using time_clock = typename proactor::time_clock;

  explicit timer_op(proactor &context);

  void set_timeout(size_t expire_ms);
  void set_timeout(size_t expire_ms, size_t interval_ms);

  void wait();
  time_clock::duration expire();

  void async_wait(func_type async_func, std::error_code &ec);

  void close(std::error_code &ec);

private:
  proactor *ctx_;

  struct timer_op_private : public operation {
    func_type func;
    time_clock::time_point expire;
    time_clock::duration interval;
    int64_t timeout_num;
    short stop;

    void complete(void *proactor, const std::error_code &result_ec,
                  size_t trans_size) override;
  } op_;

}; // class timer_op

#endif // PROACTOR_OPERATION_TIMEROP_H
