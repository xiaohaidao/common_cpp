
#ifndef PROACTOR_OPERATION_DETAIL_SENDOP_H
#define PROACTOR_OPERATION_DETAIL_SENDOP_H

#include <functional>

#include "proactor/operation/detail/Operation.h"

namespace detail {

class SendOp : public Operation {
public:
  typedef std::function<void(void *, const std::error_code &, size_t)>
      func_type;

  SendOp();

  void async_send(sockets::socket_type s, const char *buff, size_t size,
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
  sockets::socket_type socket_;
#endif // #ifndef _WIN32
};     // class SendOp

} // namespace detail

#endif // PROACTOR_OPERATION_DETAIL_SENDOP_H
