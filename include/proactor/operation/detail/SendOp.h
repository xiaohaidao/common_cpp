
#ifndef PROACTOR_OPERATION_DETAIL_SENDOP_H
#define PROACTOR_OPERATION_DETAIL_SENDOP_H

#include <functional>

#include "proactor/operation/detail/Operation.h"

namespace detail {

class send_op : public operation {
public:
  using func_type =
      std::function<void(void *, const std::error_code &, size_t)>;

  send_op();

  void async_send(void *proactor, socket_type s, const char *buff, size_t size,
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
#ifndef _WIN32
  socket_type socket_{-1};
#endif // #ifndef _WIN32
};     // class send_op

} // namespace detail

#endif // PROACTOR_OPERATION_DETAIL_SENDOP_H
