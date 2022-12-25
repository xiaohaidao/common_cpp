
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

class Select {
public:
  typedef fd_set fd_type;
#ifdef _WIN32
  typedef SOCKET socket_type;
#else
  typedef int socket_type;
#endif // _WIN32

  Select(const Select &) = delete;
  const Select &operator=(const Select &) = delete;

  Select();
  Select(std::error_code &ec);

  size_t call(QueueOp &queue);
  size_t call_one(QueueOp &queue);

  size_t run_once(QueueOp &queue, std::error_code &ec);
  size_t run_once_timeout(QueueOp &queue, size_t timeout_ms,
                          std::error_code &ec);

  void post_read(socket_type s, ReactorOp *op, std::error_code &ec);
  void post_write(socket_type s, ReactorOp *op, std::error_code &ec);
  void post_except(socket_type s, ReactorOp *op, std::error_code &ec);
  void depost(socket_type s, std::error_code &ec);

private:
  socket_type fd_;

  std::array<std::pair<socket_type, ReactorOp *>, FD_SETSIZE> map_read_op_;
  std::array<std::pair<socket_type, ReactorOp *>, FD_SETSIZE> map_write_op_;
  std::array<std::pair<socket_type, ReactorOp *>, FD_SETSIZE> map_except_op_;
  size_t map_read_op_size_;
  size_t map_write_op_size_;
  size_t map_except_op_size_;

  fd_type read_;
  fd_type write_;
  fd_type except_;
}; // class Select

#endif // REACTOR_SELECT_H
