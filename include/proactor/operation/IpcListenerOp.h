
#ifndef PROACTOR_OPERATION_IPCLISTENEROP_H
#define PROACTOR_OPERATION_IPCLISTENEROP_H

#include "ipc/PipeListener.h"
#include "proactor/operation/IpcStreamOp.h"
#include "proactor/operation/TcpListenerOp.h"
#include "proactor/operation/detail/PipeConnectOp_win.h"

class ipc_listener_op {
public:
  using func_type =
      std::function<void(const std::error_code &, const ipc_stream_op &)>;

  explicit ipc_listener_op(proactor &context);

  ipc_listener_op(const ipc_listener_op &);
  ipc_listener_op &operator=(const ipc_listener_op &);

  void bind(const char *name, std::error_code &ec);

  void async_accept(const func_type &f, std::error_code &ec);
  ipc_stream_op accept(std::error_code &ec);

  void close(std::error_code &ec);

  native_handle native() const;

private:
#if defined(_WIN32)
  proactor *ctx_;
  detail::pipe_connect_op connect_op_;
  ipc::pipe_listener pipe_;
#elif defined(__linux__)
  tcp_listener_op unix_op_;
#endif

}; // class Ipc

#endif // PROACTOR_OPERATION_IPCLISTENEROP_H
