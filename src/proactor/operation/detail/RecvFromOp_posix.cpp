
#ifdef __linux__

#include "proactor/operation/detail/RecvFromOp.h"

#include "proactor/Proactor.h"
#include "sockets/UdpSocket.h"
#include "utils/error_code.h"

namespace detail {

RecvFromOp::RecvFromOp() {}

void RecvFromOp::async_recv_from(void *proactor, socket_type s, char *buff,
                                 size_t size, func_type async_func,
                                 std::error_code &ec) {

  buff_ = {(uint32_t)size, (char *)buff};
  func_ = async_func;
  socket_ = s;
  if (proactor == nullptr) {
    std::error_code re_ec = {ENXIO, std::system_category()};
    complete(proactor, re_ec, 0);
    return;
  }
  set_event_data(READ_OP_ENUM_ONCE);
  static_cast<Proactor *>(proactor)->post(s, this, ec);
}

void RecvFromOp::complete(void *p, const std::error_code &result_ec,
                          size_t trans_size) {

  std::error_code re_ec = result_ec;
  if (func_) {
    std::pair<size_t, SocketAddr> re;
    if (!re_ec) {
      UdpSocket udp(socket_);
      re = udp.recv_from(buff_.buff, buff_.len, re_ec);
    }
    func_(p, re_ec, re.first, re.second);
  }
}

} // namespace detail

#endif // __linux__
