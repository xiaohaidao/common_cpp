
#ifndef PROACTOR_OPERATION_DETAIL_RECVFROMOP_H
#define PROACTOR_OPERATION_DETAIL_RECVFROMOP_H

#include <functional>

#include "proactor/operation/detail/Operation.h"

namespace detail {

class RecvFromOp : public Operation {
public:
  typedef std::function<void(Proactor *, const std::error_code &, size_t,
                             sockets::SocketAddr)>
      func_type;

  RecvFromOp();

  void async_recv_from(sockets::socket_type s, char *buff, size_t size,
                       func_type async_func, std::error_code &ec);

  // protected:
  // Proactor call this function
  void complete(Proactor *p, const std::error_code &result_ec,
                size_t trans_size) override;

private:
  struct Buff {
    uint32_t len;
    char *buff;
  } buff_;

  func_type func_;
  sockets::SocketAddr from_;
  int from_size_;

}; // class RecvFromOp

} // namespace detail

#endif // PROACTOR_OPERATION_DETAIL_RECVFROMOP_H
