// Copyright (C) 2022 All rights reserved.
// Email: oxox0@qq.com. Created in 202207

#ifndef IPC_SHAREDMEMORY_H
#define IPC_SHAREDMEMORY_H

#include <string>
#include <system_error>

namespace ipc {

class shared_memory {
public:
  shared_memory();
  ~shared_memory();

  static shared_memory create(const char *key, size_t mem_size,
                              std::error_code &ec);
  static shared_memory create(char *key, size_t mem_size, std::error_code &ec) {
    return create((const char *)key, mem_size, ec);
  }

  template <typename T>
  static shared_memory create(T &&key, size_t mem_size, std::error_code &ec) {
    return create(std::to_string(std::forward<T>(key)).c_str(), mem_size, ec);
  }

  static shared_memory open(const char *key, std::error_code &ec);
  static shared_memory open(char *key, std::error_code &ec) {
    return open((const char *)key, ec);
  }
  template <typename T>
  static shared_memory open(T &&key, std::error_code &ec) {
    return open(std::to_string(std::forward<T>(key)).c_str(), ec);
  }
  void *memory() const;
  // size_t size() const; // it's error on windows

  void close(std::error_code &ec);

  // do nothing on windows platform
  void remove(std::error_code &ec);

private:
  void attach(std::error_code &ec);

  void deatch(std::error_code &ec);

#if defined(_WIN32)
  using native_handle = void *;
#elif defined(__linux__)
  using native_handle = int;

  /// unlink need it
  std::string key_;
#endif // __linux__

  native_handle shmid_{0};
  void *memory_{nullptr};
  size_t size_{0};

}; // class shared_memory

} // namespace ipc

#endif // IPC_SHAREDMEMORY_H
