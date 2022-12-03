
#ifndef PROACTOR_OPERATION_SENDTOOP_H
#define PROACTOR_OPERATION_SENDTOOP_H

#include <functional>

#include "Operation.h"

class Proactor;

class SendToOp : public Operation {
public:
  typedef std::function<void(Proactor *, const std::error_code &, size_t)>
      func_type;

  SendToOp();

  void async_send_to(sockets::socket_type s, const char *buff, size_t size,
                     func_type async_func, const sockets::SocketAddr &to,
                     std::error_code &ec);

protected:
  // Proactor call this function
  void complete(Proactor *p, const std::error_code &result_ec,
                size_t trans_size) override;

private:
  struct Buff {
    uint32_t len;
    char *buff;
  } buff_;

  func_type func_;
  sockets::SocketAddr to_;

}; // class SendToOp

#endif // PROACTOR_OPERATION_SENDTOOP_H
