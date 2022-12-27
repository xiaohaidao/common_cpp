
#ifdef __linux__

#include "proactor/operation/detail/AcceptOp.h"

#include <sys/socket.h>
#include <unistd.h>

#include "proactor/Proactor.h"
#include "utils/error_code.h"

namespace detail {

AcceptOp::AcceptOp() : server_(-1) {}

void AcceptOp::async_accept(sockets::socket_type s, func_type async_func,
                            std::error_code &ec) {
  func_ = async_func;
  server_ = s;
}

void AcceptOp::complete(void *p, const std::error_code &result_ec,
                        size_t trans_size) {

  if (p) {
    std::error_code ec;
    static_cast<Proactor *>(p)->cancel(server_, ec);
  }
  std::error_code re_ec = result_ec;
  if (func_) {
    // Accept a client socket
    std::pair<sockets::socket_type, sockets::SocketAddr> ac_addr;
    if (!re_ec) {
      sockets::socket_type client = ::accept(server_, NULL, NULL);
      if (client == -1) {
        re_ec = getNetErrorCode();
      }
      ac_addr.first = client;
      if (!re_ec) {
        ac_addr.second = sockets::SocketAddr::get_remote_socket(client, re_ec);
      }
    }

    server_ = -1;

    func_(p, re_ec, std::move(ac_addr));
  }
}

} // namespace detail

#endif // __linux__
