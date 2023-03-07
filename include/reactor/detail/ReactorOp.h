
#ifndef REACTOR_DETAIL_REACTOROP_H
#define REACTOR_DETAIL_REACTOROP_H

#include "reactor/detail/OperationEpoll.h"
#include "reactor/detail/OperationPosix.h"

#ifdef _WIN32
typedef detail::OperationPosix ReactorOp;
#else
typedef detail::OperationEpoll ReactorOp;
#endif // _WIN32

#endif // REACTOR_DETAIL_REACTOROP_H