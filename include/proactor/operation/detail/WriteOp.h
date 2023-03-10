
#ifndef PROACTOR_OPERATION_DETAIL_WRITEOP_H
#define PROACTOR_OPERATION_DETAIL_WRITEOP_H

#include <functional>

#include "proactor/operation/detail/Operation.h"

namespace detail {

class WriteOp : public Operation {
public:
  typedef std::function<void(void *, const std::error_code &, size_t)>
      func_type;

  WriteOp();

  void async_write(void *proactor, socket_type s, const char *buff, size_t size,
                   func_type async_func, std::error_code &ec);

  // protected:
  // Proactor call this function
  void complete(void *proactor, const std::error_code &result_ec,
                size_t trans_size) override;

private:
#ifndef _WIN32
  native_handle fd_;
  struct Buff {
    uint32_t len;
    char *buff;
  } buff_;
#endif // #ifndef _WIN32

  func_type func_;
}; // class WriteOp

} // namespace detail

#endif // PROACTOR_OPERATION_DETAIL_WRITEOP_H
