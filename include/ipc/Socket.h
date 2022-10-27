// Copyright (C) 2022 All rights reserved.
// Email: oxox0@qq.com. Created in 202207

#include "ipc/socket/SocketAddr.h"
#include "ipc/socket/TcpListener.h"
#include "ipc/socket/TcpStream.h"
#include "ipc/socket/UdpSocket.h"

namespace {

void test_select() {
#include <sys/time.h>

  struct timeval {
    long tv_sec;  /* seconds */
    long tv_usec; /* microseconds */
  };

#include <sys/select.h>

  FD_ZERO(readfds);
  FD_SET(sd, &readfds);
  FD_CLR(sd, &readfds);
  FD_ISSET(sd, &readfds);
  select(FD_SETSIZE, &readfds, &writefds, &exceptfds, &timeout);
  ioctl();

#include <sys/epoll.h>

  epoll_create1();
  epoll_ctl();
  epoll_wait();

#include <windows.h>
  CreateIoCompletionPort();
  GetQueuedCompletionStatus();
  PostQueuedCompletionStatus();
  CloseHandle();
}

} // namespace
