
#ifndef PROACTOR_OPERATION_DETAIL_CONNECTOP_H
#define PROACTOR_OPERATION_DETAIL_CONNECTOP_H

#include <functional>

#include "proactor/operation/detail/Operation.h"

namespace detail {

class connect_op : public operation {
public:
  using func_type =
      std::function<void(void *, const std::error_code &, size_t, socket_type)>;

  connect_op();

  void async_connect(void *proactor, socket_type s, const socket_addr &addr,
                     func_type async_func, std::error_code &ec);

  // protected:
  // Proactor call this function
  void complete(void *proactor, const std::error_code &result_ec,
                size_t trans_size) override;

private:
  func_type func_;
  socket_type client_{-1};

}; // class ConnectOp

} // namespace detail

#endif // PROACTOR_OPERATION_DETAIL_CONNECTOP_H
