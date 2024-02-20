
#ifndef IPC_PIPELISTENER_H
#define IPC_PIPELISTENER_H

#include <system_error>

#include "PipeStream.h"

namespace ipc {

class PipeListener {
public:
  PipeListener();

  static PipeListener create(const char *name_pipe, std::error_code &ec);
  void create(std::error_code &ec);

  PipeStream accept(std::error_code &ec);

  void remove(std::error_code &ec);

  native_handle native() const;

private:
  char name_[256];
#ifdef _WIN32
  native_handle named_pipe_;
#else //__linux__
#endif
};

} // namespace ipc

#endif // IPC_PIPELISTENER_H
