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

  static MsgQueue get(const std::string &key, std::error_code &ec);

  static MsgQueue get(const char *key, std::error_code &ec) {
    return get(std::string(key), ec);
  }

  template <typename T> static MsgQueue get(const T &key, std::error_code &ec) {
    return get(std::to_string(key), ec);
  }

  /// 当队列满时，命令会阻塞
  void send(const char *data, size_t size, std::error_code &ec);

  void sendTimeout(const char *data, size_t size, size_t timeout_ms,
                   std::error_code &ec);

  /// 当队列空时，命令会阻塞, data_size建议不小于8192
  size_t recv(char *data, size_t data_size, std::error_code &ec);

  /// data_size建议不小于8192
  size_t recvTimeout(char *data, size_t data_size, size_t timeout_ms,
                     std::error_code &ec);

  void close(std::error_code &ec);
  void remove(std::error_code &ec);

private:
  int msgid_;

  /// mq_unlink need it
  std::string key_;

}; // class MsgQueue

} // namespace ipc

#endif // IPC_MSGQUEUE_H
