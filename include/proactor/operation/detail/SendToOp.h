
#ifndef PROACTOR_OPERATION_DETAIL_SENDTOOP_H
#define PROACTOR_OPERATION_DETAIL_SENDTOOP_H

#include <functional>

#include "proactor/operation/detail/Operation.h"

namespace detail {

class SendToOp : public Operation {
public:
  typedef std::function<void(void *, const std::error_code &, size_t)>
      func_type;

  SendToOp();

  void async_send_to(void *proactor, socket_type s, const char *buff,
                     size_t size, const SocketAddr &to, func_type async_func,
                     std::error_code &ec);

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
  SocketAddr to_;

#ifndef _WIN32
  socket_type socket_;
#endif // #ifndef _WIN32

}; // class SendToOp

} // namespace detail

#endif // PROACTOR_OPERATION_DETAIL_SENDTOOP_H
