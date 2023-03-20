
#ifndef PROACTOR_OPERATION_DETAIL_EVENTOP_POSIX_H
#define PROACTOR_OPERATION_DETAIL_EVENTOP_POSIX_H

#include <functional>

#include "proactor/operation/detail/Operation.h"

namespace detail {

class EventOp : public Operation {
public:
  typedef std::function<void(const std::error_code &, size_t)> func_type;

  EventOp();
  static EventOp create(std::error_code &ec);

  void notify(std::error_code &ec);
  void wait(std::error_code &ec);

  void close(std::error_code &ec);

  ::native_handle native_handle() const;

  void async_wait(void *proactor, func_type async_func, std::error_code &ec);

  // protected:
  // Proactor call this function
  void complete(void *proactor, const std::error_code &result_ec,
                size_t trans_size) override;

private:
  ::native_handle fd_;
  func_type func_;

}; // class EventOp

} // namespace detail

#endif // PROACTOR_OPERATION_DETAIL_EVENTOP_POSIX_H
