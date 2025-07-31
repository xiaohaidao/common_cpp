#ifndef REACTOR_DETAIL_OPERATIONWIN_H
#define REACTOR_DETAIL_OPERATIONWIN_H

#ifdef _WIN32

#include <winsock2.h>

#include <system_error>

namespace detail {

class OperationAccess;

class OperationWin : public OVERLAPPED {
public:
  OperationWin() : next_(nullptr) {
    Internal = 0;
    InternalHigh = 0;
    Offset = 0;
    OffsetHigh = 0;
    hEvent = 0;
  }

  // protected:
  virtual void complete(void *proactor, const std::error_code &result_ec,
                        size_t trans_size) = 0;

private:
  friend class OperationAccess;

  void *next_;
};

} /* namespace detail */

#endif /* _WIN32 */

#endif /* REACTOR_DETAIL_OPERATIONWIN_H */
