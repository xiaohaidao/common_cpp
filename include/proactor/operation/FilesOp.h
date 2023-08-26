
#ifndef PROACTOR_OPERATION_FILESOP_H
#define PROACTOR_OPERATION_FILESOP_H

#include <functional>

#include "proactor/Proactor.h"
#include "proactor/operation/detail/ReadOp.h"
#include "proactor/operation/detail/WriteOp.h"

class FilesOp {
public:
  typedef std::function<void(const std::error_code &, size_t)> func_type;

  FilesOp();
  explicit FilesOp(Proactor *context);
  FilesOp(Proactor *context, native_handle s);

  FilesOp(const FilesOp &);
  const FilesOp &operator=(const FilesOp &);

  /** don't support file
  void open(const char *file_path, std::error_code &ec);
  void open(const char *file_path, bool create, std::error_code &ec);
   */

  void async_read(char *buff, size_t buff_size, func_type f,
                  std::error_code &ec);
  void async_write(const char *buff, size_t buff_size, func_type f,
                   std::error_code &ec);

  size_t read(char *buff, size_t buff_size, std::error_code &ec);
  size_t write(const char *buff, size_t buff_size, std::error_code &ec);

  void close(std::error_code &ec);

  ::native_handle native_handle() const;

private:
  Proactor *ctx_;
  ::native_handle fd_;
  detail::ReadOp read_op_;
  detail::WriteOp write_op_;

}; // class FilesOp

#endif // PROACTOR_OPERATION_FILESOP_H
