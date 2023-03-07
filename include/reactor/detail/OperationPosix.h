
#ifndef REACTOR_DETAIL_OPERATIONPOSIX_H
#define REACTOR_DETAIL_OPERATIONPOSIX_H

#include <system_error>

namespace detail {

class OperationAccess;

class OperationPosix {
public:
  OperationPosix() : next_(nullptr) {}

  // owner is Reactor
  virtual void complete(void *owner, const std::error_code &result_ec,
                        size_t trans_size) = 0;

private:
  friend class detail::OperationAccess;

  void *next_;
}; // class OperationPosix

class OperationAccess {
public:
  static void set_next(void *operation, void *n) {
    static_cast<OperationPosix *>(operation)->next_ = n;
  }

  static void *next(void *operation) {
    return static_cast<OperationPosix *>(operation)->next_;
  }
};

} // namespace detail

#endif // REACTOR_DETAIL_OPERATIONPOSIX_H