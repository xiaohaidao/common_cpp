
#ifndef PROACTOR_OPERATION_DETAIL_READOP_H
#define PROACTOR_OPERATION_DETAIL_READOP_H

#include <functional>

#include "proactor/operation/detail/Operation.h"

namespace detail {

class read_op : public operation {
public:
  using func_type =
      std::function<void(void *, const std::error_code &, size_t)>;
  read_op();

  void async_read(void *proactor, func_type async_func, native_handle fd,
                  const char *buff, size_t size, std::error_code &ec);

  // protected:
  // Proactor call this function
  void complete(void *proactor, const std::error_code &result_ec,
                size_t trans_size) override;

private:
#ifndef _WIN32
  native_handle fd_{0};
  struct buff {
    uint32_t len;
    char *buff;
  } buff_{};
#endif // #ifndef _WIN32

  func_type func_;

}; // class ReadOp

} // namespace detail

#endif // PROACTOR_OPERATION_DETAIL_READOP_H
