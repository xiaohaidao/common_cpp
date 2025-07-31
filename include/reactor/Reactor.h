
#ifndef REACTOR_REACTOR_H
#define REACTOR_REACTOR_H

#include "reactor/Epoll.h"
#include "reactor/Select.h"

#ifdef _WIN32
using reactor = class select;
using reactor_select = class select;
#else
using reactor = epoll;
using reactor_select = class select;

#endif // _WIN32

#endif // REACTOR_REACTOR_H
