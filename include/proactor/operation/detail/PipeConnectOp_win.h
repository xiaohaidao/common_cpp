
#ifndef PROACTOR_OPERATION_DETAIL_PIPECONNECTOP_WIN_H
#define PROACTOR_OPERATION_DETAIL_PIPECONNECTOP_WIN_H

#include <functional>

#include "proactor/operation/detail/Operation.h"

namespace detail {

class PipeConnectOp : public Operation {
public:
  typedef std::function<void(void *p, const std::error_code &, size_t)>
      func_type;

  PipeConnectOp();

  void async_connect(void *proactor, native_handle h, func_type async_func,
                     std::error_code &ec);

  // protected:
  // Proactor call this function
  void complete(void *proactor, const std::error_code &result_ec,
                size_t trans_size) override;

private:
  func_type func_;

}; // class PipeConnectOp

} // namespace detail

#endif // PROACTOR_OPERATION_DETAIL_PIPECONNECTOP_WIN_H
