// Copyright (C) 2022 All rights reserved.
// Email: oxox0@qq.com. Created in 202210

#ifndef IPC_MSGQUEUE_H
#define IPC_MSGQUEUE_H

#include <string>
#include <system_error>

namespace ipc {

class MsgQueue {
public:
  MsgQueue();
  ~MsgQueue();

  static MsgQueue create(const std::string &key, std::error_code &ec);

  static MsgQueue create(const char *key, std::error_code &ec) {
    return create(std::string(key), ec);
  }

  template <typename T>
  static MsgQueue create(const T &key, std::error_code &ec) {
    return create(std::to_string(key), ec);
  }

  static MsgQueue open(const std::string &key, std::error_code &ec);

  static MsgQueue open(const char *key, std::error_code &ec) {
    return open(std::string(key), ec);
  }

  template <typename T>
  static MsgQueue open(const T &key, std::error_code &ec) {
    return open(std::to_string(key), ec);
  }

  // when queue is full, it will be blocked
  void send(const char *data, size_t size, std::error_code &ec);

  bool sendTimeout(const char *data, size_t size, size_t timeout_ms,
                   std::error_code &ec);

  // when queue is full, it will be blocked. data_size recommendation is not
  // less than 8192
  size_t recv(char *data, size_t data_size, std::error_code &ec);

  // data_size recommendation is not less than 8192
  size_t recvTimeout(char *data, size_t data_size, size_t timeout_ms,
                     std::error_code &ec);

  void close(std::error_code &ec);
  void remove(std::error_code &ec);

private:
  int msgid_;

  /// unlink need it
  std::string key_;

}; // class MsgQueue

} // namespace ipc

#endif // IPC_MSGQUEUE_H
