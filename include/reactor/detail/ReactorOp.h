
#ifndef REACTOR_DETAIL_REACTOROP_H
#define REACTOR_DETAIL_REACTOROP_H

#include "reactor/detail/OperationEpoll.h"
#include "reactor/detail/OperationWin.h"

#ifdef _WIN32
using reactor_op = detail::operation_win;
#else
using reactor_op = detail::operation_epoll;
#endif // _WIN32

#endif // REACTOR_DETAIL_REACTOROP_H
