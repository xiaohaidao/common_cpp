
#ifdef __linux__

#ifndef PROACTOR_OPERATION_DETAIL_POSIX_H
#define PROACTOR_OPERATION_DETAIL_POSIX_H

#include "reactor/detail/ReactorOp.h"

namespace detail {

// owner is Proactor
typedef ReactorOp OperationPosix;

} // namespace detail

#endif // PROACTOR_OPERATION_DETAIL_POSIX_H

#endif // __linux__
