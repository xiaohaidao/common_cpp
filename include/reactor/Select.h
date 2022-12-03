
#ifndef REACTOR_SELECT_H
#define REACTOR_SELECT_H

#include <functional>
#include <map>
#include <system_error>

#ifdef _WIN32
#include <winsock2.h>
#else
#endif // _WIN32

class Select {
public:
#ifdef _WIN32
  typedef SOCKET socket_type;
  typedef fd_set fd_type;
#else
  typedef int socket_type;
#endif // _WIN32

  Select();

  void run_one(std::error_code &ec);
  void run_one_timeout(size_t timeout_ms, std::error_code &ec);

  void post_read(socket_type s, std::function<void(Select &)> f);
  void post_write(socket_type s, std::function<void(Select &)> f);
  void post_except(socket_type s, std::function<void(Select &)> f);
  void depost(socket_type s);

  void wait(std::error_code &ec);
  // return false when timeout or error, otherwise return true
  bool wait_timeout(size_t timeout_ms, std::error_code &ec);

  bool readability(socket_type s);
  bool writability(socket_type s);
  bool check_except(socket_type s);

private:
  std::map<socket_type, std::function<void(Select &)> > map_read_;
  std::map<socket_type, std::function<void(Select &)> > map_write_;
  std::map<socket_type, std::function<void(Select &)> > map_except_;
  fd_type read_;
  fd_type write_;
  fd_type except_;
  fd_type readability_;
  fd_type writability_;
  fd_type check_except_;
};

#endif // REACTOR_SELECT_H
