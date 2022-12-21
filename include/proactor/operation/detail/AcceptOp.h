
#ifndef PROACTOR_OPERATION_DETAIL_ACCEPTOP_H
#define PROACTOR_OPERATION_DETAIL_ACCEPTOP_H

#include <functional>

#include "proactor/operation/detail/Operation.h"

namespace detail {

class AcceptOp : public Operation {
public:
  typedef std::function<void(
      Proactor *, const std::error_code &,
      std::pair<sockets::socket_type, sockets::SocketAddr>)>
      func_type;

  AcceptOp();

  void async_accept(sockets::socket_type s, func_type async_func,
                    std::error_code &ec);

  // protected:
  // Proactor call this function
  void complete(Proactor *p, const std::error_code &result_ec,
                size_t trans_size) override;

private:
  func_type func_;
  sockets::socket_type server_;
  sockets::socket_type client_;
  char addresses_[32];

}; // class AcceptOp

} // namespace detail

#endif // PROACTOR_OPERATION_DETAIL_ACCEPTOP_H