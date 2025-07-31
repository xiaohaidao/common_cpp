
#ifndef REACTOR_EPOLL_H
#define REACTOR_EPOLL_H

#ifdef __linux__

#include "reactor/detail/QueueOp.h"

class epoll {
public:
  epoll(const epoll &) = delete;
  const epoll &operator=(const epoll &) = delete;

  explicit epoll(std::error_code &ec);
  explicit epoll(int fd);

  void post(int fd, reactor_op *op,
            std::error_code &ec); // use op set the EPollOpEnum
  void post_read(int fd, reactor_op *op, std::error_code &ec);
  void post_write(int fd, reactor_op *op, std::error_code &ec);
  void cancel(int fd, std::error_code &ec);

  size_t call(queue_op &queue);
  size_t call_one(queue_op &queue);

  size_t run_once(queue_op &queue, std::error_code &ec);
  size_t run_once_timeout(queue_op &queue, int timeout_ms, std::error_code &ec);

  void close(std::error_code &ec);

  int native() const { return fd_; }

  void set_proactor(void *proactor);

private:
  epoll();

  int fd_{-1};
  void *proactor_{nullptr};
};

#endif // __linux__

#endif // REACTOR_EPOLL_H
