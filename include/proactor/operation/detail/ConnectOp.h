
#ifndef PROACTOR_OPERATION_DETAIL_CONNECTOP_H
#define PROACTOR_OPERATION_DETAIL_CONNECTOP_H

#include <functional>

#include "proactor/operation/detail/Operation.h"

namespace detail {

class ConnectOp : public Operation {
public:
  typedef std::function<void(void *, const std::error_code &, size_t,
                             sockets::socket_type)>
      func_type;

  ConnectOp();

  void async_connect(sockets::socket_type s, const sockets::SocketAddr &addr,
                     func_type async_func, std::error_code &ec);

  // protected:
  // Proactor call this function
  void complete(void *proactor, const std::error_code &result_ec,
                size_t trans_size) override;

private:
  func_type func_;
  sockets::socket_type client_;

}; // class ConnectOp

} // namespace detail

#endif // PROACTOR_OPERATION_DETAIL_CONNECTOP_H
