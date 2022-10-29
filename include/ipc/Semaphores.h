// Copyright (C) 2022 All rights reserved.
// Email: oxox0@qq.com. Created in 202210

#ifndef IPC_SEMAPHORES_H
#define IPC_SEMAPHORES_H

#include <string>
#include <system_error>

namespace ipc {

class Semaphores {
public:
  Semaphores();
  ~Semaphores();

  static Semaphores create(const std::string &key, std::error_code &ec);

  static Semaphores create(const char *key, std::error_code &ec) {
    return create(std::string(key), ec);
  }

  template <typename T>
  static Semaphores create(const T &key, std::error_code &ec) {
    return create(std::to_string(key), ec);
  }

  static Semaphores open(const std::string &key, std::error_code &ec);

  static Semaphores open(const char *key, std::error_code &ec) {
    return open(std::string(key), ec);
  }

  template <typename T>
  static Semaphores open(const T &key, std::error_code &ec) {
    return open(std::to_string(key), ec);
  }

  void wait(std::error_code &ec);
  bool tryWait(std::error_code &ec);
  bool tryWaitFor(size_t timeout_ms, std::error_code &ec);

  void notifyOne(std::error_code &ec);

  void close(std::error_code &ec);
  void remove(std::error_code &ec);

private:
  void *sem_;

  /// unlink need it
  std::string key_;

}; // class Semaphores

} // namespace ipc

#endif // IPC_SEMAPHORES_H
