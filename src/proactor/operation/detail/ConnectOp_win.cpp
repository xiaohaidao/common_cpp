
#ifdef _WIN32

#include "proactor/operation/detail/ConnectOp.h"

#include <mswsock.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#include "utils/error_code.h"

namespace detail {

ConnectOp::ConnectOp() : client_(INVALID_SOCKET) {}

void ConnectOp::async_connect(void *proactor, socket_type s,
                              const SocketAddr &addr, func_type async_func,
                              std::error_code &ec) {

  // if (client_ != INVALID_SOCKET && client_ != 0) {
  //   ec = {EINVAL, std::system_category()};
  //   return;
  // }
  client_ = s;
  func_ = async_func;

  DWORD numBytes = 0;
  GUID guid = WSAID_CONNECTEX;
  static LPFN_CONNECTEX ConnectExPtr = NULL;
  if (!ConnectExPtr) {
    if (::WSAIoctl(client_, SIO_GET_EXTENSION_FUNCTION_POINTER, (void *)&guid,
                   sizeof(guid), (void *)&ConnectExPtr, sizeof(ConnectExPtr),
                   &numBytes, NULL, NULL)) {
      ec = getNetErrorCode();
      complete(proactor, ec, 0);
      // assert(ec);
      return;
    }
  }

  union address_union {
    sockaddr base;
    sockaddr_in v4;
    sockaddr_in6 v6;
  } a;
  memset(&a, 0, sizeof(a));
  a.base.sa_family = addr.native_family();

  if (::bind(client_, &a.base, addr.native_addr_size()) < 0) {
    ec = getNetErrorCode();
    complete(proactor, ec, 0);
    // assert(ec);
    return;
  }

  if (ConnectExPtr(client_, (sockaddr *)addr.native_addr(),
                   addr.native_addr_size(), nullptr, 0, nullptr,
                   (OVERLAPPED *)this)) {
    std::error_code re_ec = getNetErrorCode();
    if (re_ec.value() != ERROR_IO_PENDING) {
      ec = re_ec;
      complete(proactor, ec, 0);
      // assert(ec);
      return;
    }
  }
}

void ConnectOp::complete(void *p, const std::error_code &result_ec,
                         size_t trans_size) {

  if (::setsockopt(client_, SOL_SOCKET, SO_UPDATE_CONNECT_CONTEXT, NULL, 0)) {
    // std::error_code ec = getNetErrorCode();
  }
  if (func_) {
    socket_type s = client_;
    client_ = INVALID_SOCKET;

    func_(p, result_ec, trans_size, s);
  }
}

} // namespace detail

#endif // _WIN32
