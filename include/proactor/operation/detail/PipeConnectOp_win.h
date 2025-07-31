
#ifndef PROACTOR_OPERATION_DETAIL_PIPECONNECTOP_WIN_H
#define PROACTOR_OPERATION_DETAIL_PIPECONNECTOP_WIN_H

#include <functional>

#include "proactor/operation/detail/Operation.h"

namespace detail {

class pipe_connect_op : public operation {
public:
  using func_type =
      std::function<void(void *, const std::error_code &, size_t)>;

  pipe_connect_op();

  void async_connect(void *proactor, func_type async_func,
                     const native_handle &h, std::error_code &ec);

  // protected:
  // Proactor call this function
  void complete(void *proactor, const std::error_code &result_ec,
                size_t trans_size) override;

private:
  func_type func_;

}; // class PipeConnectOp

} // namespace detail

#endif // PROACTOR_OPERATION_DETAIL_PIPECONNECTOP_WIN_H
