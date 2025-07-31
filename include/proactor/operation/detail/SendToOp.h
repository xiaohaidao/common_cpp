
#ifndef PROACTOR_OPERATION_DETAIL_SENDTOOP_H
#define PROACTOR_OPERATION_DETAIL_SENDTOOP_H

#include <functional>

#include "proactor/operation/detail/Operation.h"

namespace detail {

class send_to_op : public operation {
public:
  using func_type =
      std::function<void(void *, const std::error_code &, size_t)>;

  send_to_op();

  void async_send_to(void *proactor, socket_type s, const char *buff,
                     size_t size, const socket_addr &to, func_type async_func,
                     std::error_code &ec);

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
  socket_addr to_;

#ifndef _WIN32
  socket_type socket_{-1};
#endif // #ifndef _WIN32

}; // class send_to_op

} // namespace detail

#endif // PROACTOR_OPERATION_DETAIL_SENDTOOP_H
