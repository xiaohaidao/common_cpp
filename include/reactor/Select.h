
#ifndef REACTOR_SELECT_H
#define REACTOR_SELECT_H

#include <array>
#include <system_error>

#ifdef _WIN32
#include <winsock2.h>
#else
#include <sys/select.h>
#endif // _WIN32

#include "reactor/detail/QueueOp.h"

class select {
public:
  using fd_type = fd_set;
#ifdef _WIN32
  using socket_type = SOCKET;
#else
  using socket_type = int;
#endif // _WIN32

  select(const select &) = delete;
  const select &operator=(const select &) = delete;

  explicit select(std::error_code &ec);

  size_t call(queue_op &queue);
  size_t call_one(queue_op &queue);

  size_t run_once(queue_op &queue, std::error_code &ec);
  size_t run_once_timeout(queue_op &queue, size_t timeout_ms,
                          std::error_code &ec);

  void close(std::error_code & /*ec*/) {}

  void post_read(socket_type s, reactor_op *op, std::error_code &ec);
  void post_write(socket_type s, reactor_op *op, std::error_code &ec);
  void post_except(socket_type s, reactor_op *op, std::error_code &ec);
  void cancel(socket_type s, std::error_code &ec);

private:
  select();

  socket_type fd_;

  std::array<std::pair<socket_type, reactor_op *>, FD_SETSIZE> map_read_op_;
  std::array<std::pair<socket_type, reactor_op *>, FD_SETSIZE> map_write_op_;
  std::array<std::pair<socket_type, reactor_op *>, FD_SETSIZE> map_except_op_;
  size_t map_read_op_size_{0};
  size_t map_write_op_size_{0};
  size_t map_except_op_size_{0};

  fd_type read_;
  fd_type write_;
  fd_type except_;
}; // class Select

#endif // REACTOR_SELECT_H
