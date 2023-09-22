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

  // the initial number is 1
  static Semaphores create(const char *key, std::error_code &ec,
                           unsigned int number = 1);

  static Semaphores create(char *key, std::error_code &ec,
                           unsigned int number = 1) {

    return create((const char *)key, ec, number);
  }

  template <typename T>
  static Semaphores create(T &&key, std::error_code &ec,
                           unsigned int number = 1) {
    return create(std::to_string(std::forward<T>(key)).c_str(), ec, number);
  }

  static Semaphores open(const char *key, std::error_code &ec);
  static Semaphores open(char *key, std::error_code &ec) {
    return open((const char *)key, ec);
  }

  template <typename T> static Semaphores open(T &&key, std::error_code &ec) {
    return open(std::to_string(std::forward<T>(key)).c_str(), ec);
  }

  void wait(std::error_code &ec);
  bool tryWait(std::error_code &ec);
  bool tryWaitFor(size_t timeout_ms, std::error_code &ec);

  void notifyOne(std::error_code &ec);

  void close(std::error_code &ec);

  // do nothing on windows platform
  void remove(std::error_code &ec);

private:
  void *sem_;

#if defined(__linux__)
  /// unlink need it
  std::string key_;
#endif // __linux__

}; // class Semaphores

} // namespace ipc

#endif // IPC_SEMAPHORES_H
