
#ifndef PROACTOR_OPERATION_IPCLISTENEROP_H
#define PROACTOR_OPERATION_IPCLISTENEROP_H

#include "ipc/PipeListener.h"
#include "proactor/operation/IpcStreamOp.h"
#include "proactor/operation/TcpListenerOp.h"
#include "proactor/operation/detail/PipeConnectOp_win.h"

class IpcListenerOp {
public:
  typedef std::function<void(const std::error_code &, const IpcStreamOp &)>
      func_type;

  explicit IpcListenerOp(Proactor &context);

  IpcListenerOp(const IpcListenerOp &);
  IpcListenerOp &operator=(const IpcListenerOp &);

  void bind(const char *name, std::error_code &ec);

  void async_accept(const func_type &f, std::error_code &ec);
  IpcStreamOp accept(std::error_code &ec);

  void close(std::error_code &ec);

  native_handle native() const;

private:
#if defined(_WIN32)
  Proactor *ctx_;
  detail::PipeConnectOp connect_op_;
  ipc::PipeListener pipe_;
#elif defined(__linux__)
  TcpListenerOp unix_op_;
#endif

}; // class Ipc

#endif // PROACTOR_OPERATION_IPCLISTENEROP_H
