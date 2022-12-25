
#ifdef __linux__

#ifndef REACTOR_DETAIL_OPERATIONEPOLL_H
#define REACTOR_DETAIL_OPERATIONEPOLL_H

#include "reactor/detail/OperationPosix.h"

namespace detail {

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
