
#ifndef PROACTOR_OPERATION_EVENTOP_H
#define PROACTOR_OPERATION_EVENTOP_H

#include <functional>

#ifdef __linux__
#include "proactor/operation/detail/EventOp_posix.h"
#endif
#include "proactor/Proactor.h"

class event_op {
public:
  using func_type = std::function<void(const std::error_code &, size_t)>;

  event_op();
  explicit event_op(proactor *context);
  ~event_op();

  event_op(const event_op &);
  event_op &operator=(const event_op &);

  void async_notify(const func_type &async_func, std::error_code &ec);

private:
  proactor *ctx_{nullptr};

#ifdef __linux__
  detail::event_op op_;
#else
  class Event : public operation {
  public:
    void async_notify(void *proactor, const func_type &async_func,
                      std::error_code &ec);

    // protected:
    // Proactor call this function
    void complete(void *, const std::error_code &result_ec,
                  size_t trans_size) override;

  private:
    func_type func_;
  } op_;
#endif

}; // class event_op

#endif // PROACTOR_OPERATION_EVENTOP_H
