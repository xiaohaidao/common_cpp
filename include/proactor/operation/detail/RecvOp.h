
#ifndef PROACTOR_OPERATION_DETAIL_RECVOP_H
#define PROACTOR_OPERATION_DETAIL_RECVOP_H

#include <functional>

#include "proactor/operation/detail/Operation.h"

namespace detail {

class RecvOp : public Operation {
public:
  typedef std::function<void(void *, const std::error_code &, size_t)>
      func_type;

  RecvOp();

  void async_recv(void *proactor, socket_type s, char *buff, size_t size,
                  func_type async_func, std::error_code &ec);

  // protected:
  // Proactor call this function
  void complete(void *proactor, const std::error_code &result_ec,
                size_t trans_size) override;

private:
  struct Buff {
    uint32_t len;
    char *buff;
  } buff_;

  func_type func_;
#ifndef _WIN32
  socket_type socket_;
#endif // #ifndef _WIN32

}; // class RecvOP

} // namespace detail

#endif // PROACTOR_OPERATION_DETAIL_RECVOP_H
