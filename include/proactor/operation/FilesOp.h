
#ifndef PROACTOR_OPERATION_FILESOP_H
#define PROACTOR_OPERATION_FILESOP_H

#include <functional>

#include "proactor/Proactor.h"
#include "proactor/operation/detail/ReadOp.h"
#include "proactor/operation/detail/WriteOp.h"

class files_op {
public:
  using func_type = std::function<void(const std::error_code &, size_t)>;

  files_op();
  explicit files_op(proactor *context);
  files_op(proactor *context, native_handle s);

  files_op(const files_op &);
  files_op &operator=(const files_op &);

  /** don't support file
  void open(const char *file_path, std::error_code &ec);
  void open(const char *file_path, bool create, std::error_code &ec);
   */

  void async_read(char *buff, size_t buff_size, const func_type &f,
                  std::error_code &ec);
  void async_write(const char *buff, size_t buff_size, const func_type &f,
                   std::error_code &ec);

  size_t read(char *buff, size_t buff_size, std::error_code &ec);
  size_t write(const char *buff, size_t buff_size, std::error_code &ec);

  void close(std::error_code &ec);

  ::native_handle native() const;

private:
  proactor *ctx_{nullptr};
  ::native_handle fd_{0};
  detail::read_op read_op_;
  detail::write_op write_op_;

}; // class files_op

#endif // PROACTOR_OPERATION_FILESOP_H
