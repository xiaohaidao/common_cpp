
#ifndef PROACTOR_OPERATION_DETAIL_RECVFROMOP_H
#define PROACTOR_OPERATION_DETAIL_RECVFROMOP_H

#include <functional>

#include "proactor/operation/detail/Operation.h"

namespace detail {

class recv_from_op : public operation {
public:
  using func_type = std::function<void(void *, const std::error_code &, size_t,
                                       const socket_addr &)>;

  recv_from_op();

  void async_recv_from(void *proactor, socket_type s, char *buff, size_t size,
                       func_type async_func, std::error_code &ec);

  // protected:
  // Proactor call this function
  void complete(void *proactor, const std::error_code &result_ec,
                size_t trans_size) override;

private:
  struct buff {
    uint32_t len;
    char *buff;
  } buff_;

  func_type func_;
  socket_addr from_;

#ifdef _WIN32
  int from_size_;
#else
  socket_type socket_;
#endif // #ifdef _WIN32

}; // class recv_from_op

} // namespace detail

#endif // PROACTOR_OPERATION_DETAIL_RECVFROMOP_H
