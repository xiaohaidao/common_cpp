
#ifndef PROACTOR_OPERATION_DETAIL_OPERATION_H
#define PROACTOR_OPERATION_DETAIL_OPERATION_H

#include "proactor/operation/detail/Operation_posix.h"
#include "proactor/operation/detail/Operation_win.h"
#include "sockets/SocketAddr.h"

#ifdef _WIN32
typedef detail::OperationWin Operation;
#else
typedef detail::OperationPosix Operation;
#endif // _WIN32

#endif // PROACTOR_OPERATION_DETAIL_OPERATION_H
