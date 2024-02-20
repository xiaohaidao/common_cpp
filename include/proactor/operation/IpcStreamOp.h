
#ifndef PROACTOR_OPERATION_IPCSTREAMOP_H
#define PROACTOR_OPERATION_IPCSTREAMOP_H

#include "ipc/PipeStream.h"
#include "proactor/operation/TcpStreamOp.h"
#include "proactor/operation/detail/ReadOp.h"
#include "proactor/operation/detail/WriteOp.h"

class IpcStreamOp {
public:
  typedef std::function<void(const std::error_code &, size_t)> func_type;

  explicit IpcStreamOp(Proactor *context);

#if defined(_WIN32)
  explicit IpcStreamOp(Proactor *context, const ipc::PipeStream &pipe);
#elif defined(__linux__)
  explicit IpcStreamOp(const TcpStreamOp &tcp);
#endif

  void connect(const char *name, std::error_code &ec);

  void async_read(char *buff, size_t buff_size, func_type f,
                  std::error_code &ec);
  void async_write(const char *buff, size_t buff_size, func_type f,
                   std::error_code &ec);
  size_t read(char *buff, size_t size, std::error_code &ec);
  size_t write(const char *buff, size_t size, std::error_code &ec);

  void close(std::error_code &ec);

  native_handle native() const;

private:
#if defined(_WIN32)
  Proactor *ctx_;

  detail::ReadOp read_;
  detail::WriteOp write_;
  ipc::PipeStream pipe_;
#elif defined(__linux__)
  TcpStreamOp tcp_;
#endif

}; // class Ipc

#endif // PROACTOR_OPERATION_IPCSTREAMOP_H
