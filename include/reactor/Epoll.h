
#ifndef REACTOR_EPOLL_H
#define REACTOR_EPOLL_H

#ifdef __linux__

#include "reactor/detail/QueueOp.h"

class Epoll {
public:
  Epoll(const Epoll &) = delete;
  const Epoll &operator=(const Epoll &) = delete;

  Epoll();
  Epoll(std::error_code &ec);

  enum OpEnum {
    EPOLLIN = 0x001,     // readf
    EPOLLPRI = 0x002,    // out-of-band data or other
    EPOLLOUT = 0x004,    // write
    EPOLLERR = 0x008,    // error
    EPOLLHUP = 0x010,    // remote closed read
    EPOLLRDHUP = 0x2000, // local closed write

    EPOLLET = 1u << 31 // EdgeTriggered
  };
#define READ_OP_ENUM (EPOLLIN | EPOLLERR | EPOLLHUP | EPOLLET)
#define WRITE_OP_ENUM (EPOLLOUT | EPOLLRDHUP | EPOLLHUP)

  void post(int fd, ReactorOp *op, int or_op_enum, std::error_code &ec);
  void post_read(int fd, ReactorOp *op, std::error_code &ec);
  void post_write(int fd, ReactorOp *op, std::error_code &ec);
  void depost(int fd, std::error_code &ec);

  size_t call(QueueOp &queue);
  size_t call_one(QueueOp &queue);

  size_t run_once(QueueOp &queue, std::error_code &ec);
  size_t run_once_timeout(QueueOp &queue, int timeout_ms, std::error_code &ec);

private:
  int fd_;
};

#endif // __linux__

#endif // REACTOR_EPOLL_H
