
#ifndef REACTOR_DETAIL_OPERATIONEPOLL_H
#define REACTOR_DETAIL_OPERATIONEPOLL_H

#ifdef __linux__

#include "reactor/detail/OperationPosix.h"

namespace detail {

enum OpEnum {
  kEpollin = 0x001,     // readf
  kEpollprii = 0x002,   // out-of-band data or other
  kEpollout = 0x004,    // write
  kEpollerr = 0x008,    // error
  kEpollhup = 0x010,    // remote closed read
  kEpollrdhup = 0x2000, // local closed write

  kEpolloneshot = 1u << 30, // one-shot notification
  kEpollet = 1u << 31       // EdgeTriggered
};

#define READ_OP_ENUM                                                           \
  (::detail::kEpollin | ::detail::kEpollerr | ::detail::kEpollhup |            \
   ::detail::kEpollet)
#define WRITE_OP_ENUM                                                          \
  (::detail::kEpollout | ::detail::kEpollrdhup | ::detail::kEpollhup)
#define READ_OP_ET_ENUM_ONCE                                                   \
  (::detail::kEpollin | ::detail::kEpollerr | ::detail::kEpollhup |            \
   ::detail::kEpolloneshot | ::detail::kEpollet)
#define READ_OP_ENUM_ONCE                                                      \
  (::detail::kEpollin | ::detail::kEpollerr | ::detail::kEpollhup |            \
   ::detail::kEpolloneshot)
#define WRITE_OP_ENUM_ONCE                                                     \
  (((::detail::kEpollout | ::detail::kEpollrdhup | ::detail::kEpollhup) |      \
    ::detail::kEpolloneshot))

class operation_epoll : public operation_posix {
public:
  operation_epoll() : operation_posix() {}

  // protected:
  void set_event_data(int event) { event_ = event; }

  int get_event_data() const { return event_; }

private:
  int event_{0};
}; /* class */

} // namespace detail

#endif // __linux__

#endif // REACTOR_DETAIL_OPERATIONEPOLL_H
