
#ifdef _WIN32

#include "proactor/operation/detail/AcceptOp.h"

#include <mswsock.h>
#include <winsock2.h>

#include "utils/error_code.h"

namespace detail {

AcceptOp::AcceptOp() : server_(INVALID_SOCKET), client_(INVALID_SOCKET) {}

void AcceptOp::async_accept(void *proactor, socket_type s, func_type async_func,
                            std::error_code &ec) {
  if (client_ == INVALID_SOCKET || client_ == 0) {
    client_ = sockets::socket(kUnspecified, kStream, kTCP, ec);
    if (ec) {
      return;
    }
  }
  func_ = async_func;
  server_ = s;

  DWORD numBytes = 0;
  GUID guid = WSAID_ACCEPTEX;
  static LPFN_ACCEPTEX AcceptExPtr = NULL;
  if (!AcceptExPtr) {
    if (::WSAIoctl(client_, SIO_GET_EXTENSION_FUNCTION_POINTER, (void *)&guid,
                   sizeof(guid), (void *)&AcceptExPtr, sizeof(AcceptExPtr),
                   &numBytes, NULL, NULL)) {
      ec = getNetErrorCode();
      complete(proactor, ec, 0);
      // assert(ec);
      return;
    }
  }

  DWORD p = 0;
  if (!AcceptExPtr(s, client_, addresses_, 0, 0, 32, &p, (OVERLAPPED *)this)) {
    std::error_code re_ec = getNetErrorCode();
    if (re_ec.value() != ERROR_IO_PENDING && re_ec.value() != 0) {
      ec = re_ec;
      complete(proactor, ec, 0);
      // assert(ec);
      return;
    }
  }
}

void AcceptOp::complete(void *p, const std::error_code &result_ec,
                        size_t trans_size) {

  if (setsockopt(client_, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT,
                 (char *)&server_, sizeof(server_))) {
    // std::error_code ec = getNetErrorCode();
  }
  if (func_) {
    std::pair<socket_type, SocketAddr> ac_addr;
    ac_addr.first = client_;
    memcpy(ac_addr.second.native_addr(), addresses_,
           ac_addr.second.native_addr_size());

    memset(addresses_, 0, sizeof(addresses_));
    client_ = INVALID_SOCKET;
    // server_ = INVALID_SOCKET;

    func_(p, result_ec, ac_addr);
  }
}

} // namespace detail

#endif // _WIN32
