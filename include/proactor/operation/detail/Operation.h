
#ifndef PROACTOR_OPERATION_DETAIL_OPERATION_H
#define PROACTOR_OPERATION_DETAIL_OPERATION_H

#include "proactor/operation/detail/Operation_win.h"
#include "reactor/detail/ReactorOp.h"
#include "sockets/SocketAddr.h"

#ifdef _WIN32
typedef detail::OperationWin Operation;
#else
// owner is Proactor
typedef ReactorOp Operation;
#endif // _WIN32

#ifdef _WIN32
typedef void *native_handle;
#else  // _WIN32
typedef int native_handle;
#endif // _WIN32

#endif // PROACTOR_OPERATION_DETAIL_OPERATION_H
