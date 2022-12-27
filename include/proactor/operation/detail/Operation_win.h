
#ifdef _WIN32

#ifndef PROACTOR_OPERATION_DETAIL_WIN_H
#define PROACTOR_OPERATION_DETAIL_WIN_H

#include <winsock2.h>

#include <system_error>

namespace detail {

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
};

} // namespace detail

#endif // PROACTOR_OPERATION_DETAIL_WIN_H

#endif // _WIN32
