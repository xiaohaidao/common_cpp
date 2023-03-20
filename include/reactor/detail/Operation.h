
#ifndef REACTOR_DETAIL_OPERATION_H
#define REACTOR_DETAIL_OPERATION_H

#include "reactor/detail/ReactorOp.h"
#include "sockets/SocketAddr.h"

// owner is Proactor
typedef ReactorOp Operation;

#ifdef _WIN32
typedef void *native_handle;
#else  // _WIN32
typedef int native_handle;
#endif // _WIN32

namespace detail {

#ifdef _WIN32

class OperationAccess {
public:
  static void set_next(void *operation, void *n) {
    static_cast<OperationWin *>(operation)->next_ = n;
  }

  static void *next(void *operation) {
    return static_cast<OperationWin *>(operation)->next_;
  }
};

#else

class OperationAccess {
public:
  static void set_next(void *operation, void *n) {
    static_cast<OperationPosix *>(operation)->next_ = n;
  }

  static void *next(void *operation) {
    return static_cast<OperationPosix *>(operation)->next_;
  }
};

#endif // _WIN32

} // namespace detail

#endif // REACTOR_DETAIL_OPERATION_H
