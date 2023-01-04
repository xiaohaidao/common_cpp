
#ifndef PROACTOR_OPERATION_DETAIL_ACCEPTOP_H
#define PROACTOR_OPERATION_DETAIL_ACCEPTOP_H

#include <functional>

#include "proactor/operation/detail/Operation.h"

namespace detail {

class AcceptOp : public Operation {
public:
  typedef std::function<void(void *, const std::error_code &,
                             std::pair<socket_type, SocketAddr>)>
      func_type;

  AcceptOp();

  void async_accept(socket_type s, func_type async_func, std::error_code &ec);

  // protected:
  // Proactor call this function
  void complete(void *proactor, const std::error_code &result_ec,
                size_t trans_size) override;

private:
  func_type func_;
  socket_type server_;
#ifdef _WIN32
  socket_type client_;
  char addresses_[32];
#endif // _WIN32

}; // class AcceptOp

} // namespace detail

#endif // PROACTOR_OPERATION_DETAIL_ACCEPTOP_H
