
#ifdef __linux__

#include <utility>

#include "proactor/operation/detail/RecvFromOp.h"

#include "proactor/Proactor.h"
#include "sockets/UdpSocket.h"
#include "utils/error_code.h"

namespace detail {

recv_from_op::recv_from_op() = default;

void recv_from_op::async_recv_from(void *p, socket_type s, char *buff,
                                   size_t size, func_type async_func,
                                   std::error_code &ec) {

  buff_ = {(uint32_t)size, (char *)buff};
  func_ = std::move(async_func);
  socket_ = s;
  if (p == nullptr) {
    std::error_code re_ec = {ENXIO, std::system_category()};
    complete(p, re_ec, 0);
    return;
  }
  set_event_data(READ_OP_ENUM_ONCE);
  static_cast<proactor *>(p)->post(s, this, ec);
}

void recv_from_op::complete(void *p, const std::error_code &result_ec,
                            size_t /*trans_size*/) {

  std::error_code re_ec = result_ec;
  if (func_) {
    std::pair<size_t, socket_addr> re;
    if (!re_ec) {
      udp_socket udp(socket_);
      re = udp.recv_from(buff_.buff, buff_.len, re_ec);
    }
    auto tmp = std::move(func_);
    tmp(p, re_ec, re.first, re.second);
  }
}

} // namespace detail

#endif // __linux__
