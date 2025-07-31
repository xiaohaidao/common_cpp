
#ifndef PROACTOR_OPERATION_DETAIL_WRITEOP_H
#define PROACTOR_OPERATION_DETAIL_WRITEOP_H

#include <functional>

#include "proactor/operation/detail/Operation.h"

namespace detail {

class write_op : public operation {
public:
  using func_type =
      std::function<void(void *, const std::error_code &, size_t)>;
  write_op();

  void async_write(void *proactor, func_type async_func, native_handle s,
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
}; // class WriteOp

} // namespace detail

#endif // PROACTOR_OPERATION_DETAIL_WRITEOP_H
