
#ifdef _WIN32

#include "proactor/operation/detail/PipeConnectOp_win.h"

#include <namedpipeapi.h>

#include "utils/error_code.h"

namespace detail {

PipeConnectOp::PipeConnectOp() {}

void PipeConnectOp::async_connect(void *proactor, native_handle h,
                                  func_type async_func, std::error_code &ec) {

  func_ = async_func;
  if (!::ConnectNamedPipe(h, (LPOVERLAPPED)this)) {
    std::error_code re_ec = get_net_error_code();
    if (re_ec.value() == ERROR_PIPE_CONNECTED) {
      complete(proactor, ec, 0);
    } else if (re_ec.value() != ERROR_IO_PENDING && re_ec.value() != 0) {
      ec = re_ec;
      complete(proactor, ec, 0);
    }
  }
}

void PipeConnectOp::complete(void *p, const std::error_code &result_ec,
                             size_t trans_size) {

  if (func_) {
    auto tmp = std::move(func_);
    tmp(p, result_ec, trans_size);
  }
}

} // namespace detail

#endif // _WIN32
