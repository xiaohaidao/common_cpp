
#ifndef REACTOR_REACTOR_H
#define REACTOR_REACTOR_H

#include "reactor/Epoll.h"
#include "reactor/Select.h"

#ifdef _WIN32
typedef Select Reactor;
typedef Select ReactorSelect;
#else
typedef Epoll Reactor;
typedef Select ReactorSelect;

#endif // _WIN32

#endif // REACTOR_REACTOR_H
