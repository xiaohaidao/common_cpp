
#ifdef _WIN32

#ifndef PROACTOR_OPERATION_WIN_H
#define PROACTOR_OPERATION_WIN_H

#include <system_error>

class OperationWin : public OVERLAPPED {
  friend class Proactor;

protected:
  virtual void complete(Proactor *p, const std::error_code &result_ec,
                        size_t trans_size) = 0;
};

#endif // PROACTOR_OPERATION_WIN_H

#endif // _WIN32
