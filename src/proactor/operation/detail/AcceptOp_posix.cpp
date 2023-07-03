
#ifdef __linux__

#include "proactor/operation/detail/AcceptOp.h"

#include "proactor/Proactor.h"
#include "sockets/TcpListener.h"
#include "utils/error_code.h"

namespace detail {

AcceptOp::AcceptOp() : server_(-1) {}

void AcceptOp::async_accept(void *proactor, socket_type s, func_type async_func,
                            std::error_code &ec) {
  func_ = async_func;
  server_ = s;
  if (proactor == nullptr) {
    std::error_code re_ec = {ENXIO, std::system_category()};
    complete(proactor, re_ec, 0);
    return;
  }
  set_event_data(READ_OP_ET_ENUM_ONCE);
  static_cast<Proactor *>(proactor)->post(s, this, ec);
}

void AcceptOp::complete(void *p, const std::error_code &result_ec,
                        size_t trans_size) {

  std::error_code re_ec = result_ec;
  if (func_) {
    // Accept a client socket
    std::pair<socket_type, SocketAddr> ac_addr;
    if (!re_ec) {
      TcpListener listen(server_);
      std::pair<TcpStream, SocketAddr> ret = listen.accept(re_ec);
      ac_addr.first = ret.first.native_handle();
      ac_addr.second = ret.second;
    }
    func_(p, re_ec, ac_addr);
  }
}

} // namespace detail

#endif // __linux__
