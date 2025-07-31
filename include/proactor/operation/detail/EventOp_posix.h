
#ifndef PROACTOR_OPERATION_DETAIL_EVENTOP_POSIX_H
#define PROACTOR_OPERATION_DETAIL_EVENTOP_POSIX_H

#include <functional>

#include "proactor/operation/detail/Operation.h"

namespace detail {

class event_op : public operation {
public:
  using func_type = std::function<void(const std::error_code &, size_t)>;

  event_op();
  static event_op create(std::error_code &ec);

  void notify(std::error_code &ec);
  uint64_t wait(std::error_code &ec);

  void close(std::error_code &ec);

  ::native_handle native() const;

  void async_wait(void *proactor, func_type async_func, std::error_code &ec);

  // protected:
  // Proactor call this function
  void complete(void *proactor, const std::error_code &result_ec,
                size_t trans_size) override;

private:
  ::native_handle fd_{-1};
  func_type func_;

}; // class event_op

} // namespace detail

#endif // PROACTOR_OPERATION_DETAIL_EVENTOP_POSIX_H
