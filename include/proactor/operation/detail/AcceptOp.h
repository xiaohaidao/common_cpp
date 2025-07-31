
#ifndef PROACTOR_OPERATION_DETAIL_ACCEPTOP_H
#define PROACTOR_OPERATION_DETAIL_ACCEPTOP_H

#include <functional>

#include "proactor/operation/detail/Operation.h"

namespace detail {

class accept_op : public operation {
public:
  using func_type =
      std::function<void(void *, const std::error_code &,
                         const std::pair<socket_type, socket_addr> &)>;

  accept_op();

  void async_accept(void *proactor, socket_type s, func_type async_func,
                    std::error_code &ec);

  // protected:
  // Proactor call this function
  void complete(void *proactor, const std::error_code &result_ec,
                size_t trans_size) override;

private:
  func_type func_;
  socket_type server_{-1};
#ifdef _WIN32
  socket_type client_;
  char addresses_[32];
#endif // _WIN32

}; // class accept_op

} // namespace detail

#endif // PROACTOR_OPERATION_DETAIL_ACCEPTOP_H
