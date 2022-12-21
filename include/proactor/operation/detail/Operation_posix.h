
#ifdef __linux__

#ifndef PROACTOR_OPERATION_DETAIL_POSIX_H
#define PROACTOR_OPERATION_DETAIL_POSIX_H

#include <system_error>

class Proactor;

namespace detail {

class OperationPosix {
  friend class Proactor;

public:
protected:
  virtual void complete(Proactor *p, const std::error_code &result_ec,
                        size_t trans_size) = 0;

  // int handle_key;

  char *data_;
  size_t data_size_;

  // size_t trans_size_;

  // bool(&handle_key, &overlapp, &trans_size, &ec);
};

} // namespace detail

#endif // PROACTOR_OPERATION_DETAIL_POSIX_H

#endif // __linux__
