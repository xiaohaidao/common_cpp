
#ifdef __linux__

#include <utility>

#include "proactor/operation/detail/AcceptOp.h"

#include "proactor/Proactor.h"
#include "sockets/TcpListener.h"
#include "utils/error_code.h"

namespace detail {

accept_op::accept_op() = default;

void accept_op::async_accept(void *proactor, socket_type s,
                             func_type async_func, std::error_code &ec) {
  func_ = std::move(async_func);
  server_ = s;
  if (proactor == nullptr) {
    std::error_code re_ec = {ENXIO, std::system_category()};
    complete(proactor, re_ec, 0);
    return;
  }
  set_event_data(READ_OP_ET_ENUM_ONCE);
  static_cast< ::proactor *>(proactor)->post(s, this, ec);
}

void accept_op::complete(void *p, const std::error_code &result_ec,
                         size_t /*trans_size*/) {

  std::error_code re_ec = result_ec;
  if (func_) {
    // Accept a client socket
    std::pair<socket_type, socket_addr> ac_addr;
    if (!re_ec) {
      tcp_listener listen(server_);
      std::pair<tcp_stream, socket_addr> ret = listen.accept(re_ec);
      ac_addr.first = ret.first.native();
      ac_addr.second = ret.second;
    }
    auto tmp = std::move(func_);
    tmp(p, re_ec, ac_addr);
  }
}

} // namespace detail

#endif // __linux__
