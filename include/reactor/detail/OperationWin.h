
#ifdef _WIN32

#ifndef REACTOR_OPERATION_DETAIL_OPERATIONWIN_H
#define REACTOR_OPERATION_DETAIL_OPERATIONWIN_H

#include <winsock2.h>

#include <system_error>

namespace detail {

class OperationAccess;

class OperationWin : public OVERLAPPED {
public:
  OperationWin() {
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

} // namespace detail

#endif // REACTOR_OPERATION_DETAIL_OPERATIONWIN_H

#endif // _WIN32
