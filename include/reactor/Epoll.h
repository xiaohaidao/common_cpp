
#ifndef REACTOR_EPOLL_H
#define REACTOR_EPOLL_H

#ifdef __linux__

#include "reactor/detail/QueueOp.h"

class Epoll {
public:
  Epoll(const Epoll &) = delete;
  const Epoll &operator=(const Epoll &) = delete;

  explicit Epoll(std::error_code &ec);
  explicit Epoll(int fd);

  void post(int fd, ReactorOp *op,
            std::error_code &ec); // use op set the EPollOpEnum
  void post_read(int fd, ReactorOp *op, std::error_code &ec);
  void post_write(int fd, ReactorOp *op, std::error_code &ec);
  void cancel(int fd, std::error_code &ec);

  size_t call(QueueOp &queue);
  size_t call_one(QueueOp &queue);

  size_t run_once(QueueOp &queue, std::error_code &ec);
  size_t run_once_timeout(QueueOp &queue, int timeout_ms, std::error_code &ec);

  void close(std::error_code &ec);

  int native() const { return fd_; }

  void set_proactor(void *proactor);

private:
  Epoll();

  int fd_;
  void *proactor_;
};

#endif // __linux__

#endif // REACTOR_EPOLL_H
