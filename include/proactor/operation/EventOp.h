
#ifndef PROACTOR_OPERATION_EVENTOP_H
#define PROACTOR_OPERATION_EVENTOP_H

#include <functional>

#include "proactor/Proactor.h"

class EventOp {
public:
  typedef std::function<void(const std::error_code &, size_t)> func_type;

  EventOp();
  explicit EventOp(Proactor *context);

  EventOp(const EventOp &);
  EventOp &operator=(const EventOp &);

  void async_notify(const func_type &async_func, std::error_code &ec);

private:
  Proactor *ctx_;

  class Event : public Operation {
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

}; // class EventOp

#endif // PROACTOR_OPERATION_EVENTOP_H
