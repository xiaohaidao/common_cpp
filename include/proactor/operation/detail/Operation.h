
#ifndef PROACTOR_OPERATION_DETAIL_H
#define PROACTOR_OPERATION_DETAIL_H

#include "Operation_posix.h"
#include "Operation_win.h"
#include "sockets/SocketAddr.h"

#ifdef _WIN32
typedef OperationWin Operation;
#else
typedef OperationPosix Operation;
#endif // _WIN32

#endif // PROACTOR_OPERATION_DETAIL_H
