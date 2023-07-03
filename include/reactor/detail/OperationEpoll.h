
#ifdef __linux__

#ifndef REACTOR_DETAIL_OPERATIONEPOLL_H
#define REACTOR_DETAIL_OPERATIONEPOLL_H

#include "reactor/detail/OperationPosix.h"

namespace detail {

enum OpEnum {
  EPOLLIN = 0x001,     // readf
  EPOLLPRI = 0x002,    // out-of-band data or other
  EPOLLOUT = 0x004,    // write
  EPOLLERR = 0x008,    // error
  EPOLLHUP = 0x010,    // remote closed read
  EPOLLRDHUP = 0x2000, // local closed write

  EPOLLONESHOT = 1u << 30, // one-shot notification
  EPOLLET = 1u << 31       // EdgeTriggered
};

#define READ_OP_ENUM                                                           \
  (::detail::EPOLLIN | ::detail::EPOLLERR | ::detail::EPOLLHUP |               \
   ::detail::EPOLLET)
#define WRITE_OP_ENUM                                                          \
  (::detail::EPOLLOUT | ::detail::EPOLLRDHUP | ::detail::EPOLLHUP)
#define READ_OP_ET_ENUM_ONCE                                                   \
  (::detail::EPOLLIN | ::detail::EPOLLERR | ::detail::EPOLLHUP |               \
   ::detail::EPOLLONESHOT | ::detail::EPOLLET)
#define READ_OP_ENUM_ONCE                                                      \
  (::detail::EPOLLIN | ::detail::EPOLLERR | ::detail::EPOLLHUP |               \
   ::detail::EPOLLONESHOT)
#define WRITE_OP_ENUM_ONCE                                                     \
  (::detail::EPOLLOUT | ::detail::EPOLLRDHUP | ::detail::EPOLLHUP) |           \
      ::detail::EPOLLONESHOT

class OperationEpoll : public OperationPosix {
public:
  OperationEpoll() : OperationPosix(), event_(0) {}

  // protect:
  void set_event_data(int event) { event_ = event; }

  int get_event_data() const { return event_; }

private:
  int event_;
}; // class OperationEpoll

} // namespace detail

#endif // REACTOR_DETAIL_OPERATIONEPOLL_H

#endif // __linux__
