
#ifndef CONTEXT_SOCKETOP_H
#define CONTEXT_SOCKETOP_H

#include <functional>
#include <system_error>

#include "proactor/Operation.h"

class SocketOp {
public:
#ifdef _WIN32
  typedef void *native_handle;
#else  // _WIN32
  typedef pid_t native_handle;
#endif // _WIN32

  static SocketOp create(std::error_code &ec);

  void asyncRead(std::function<void()> func);
  void asyncWrite(std::function<void()> func);
  void asyncAccept(std::function<void()> func);
};

#endif // CONTEXT_SOCKETOP_H
