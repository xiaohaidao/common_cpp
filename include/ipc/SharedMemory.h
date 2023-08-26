// Copyright (C) 2022 All rights reserved.
// Email: oxox0@qq.com. Created in 202207

#ifndef IPC_SHAREDMEMORY_H
#define IPC_SHAREDMEMORY_H

#include <string>
#include <system_error>

namespace ipc {

class SharedMemory {
public:
  SharedMemory();
  ~SharedMemory();

  static SharedMemory create(const std::string &key, size_t mem_size,
                             std::error_code &ec);

  static SharedMemory create(const char *key, size_t mem_size,
                             std::error_code &ec) {
    return create(std::string(key), mem_size, ec);
  }

  static SharedMemory create(char *key, size_t mem_size, std::error_code &ec) {
    return create(std::string(key), mem_size, ec);
  }

  template <typename T>
  static SharedMemory create(const T &key, size_t mem_size,
                             std::error_code &ec) {
    return create(std::to_string(key), mem_size, ec);
  }

  static SharedMemory open(const std::string &key, std::error_code &ec);

  static SharedMemory open(const char *key, std::error_code &ec) {
    return open(std::string(key), ec);
  }

  static SharedMemory open(char *key, std::error_code &ec) {
    return open(std::string(key), ec);
  }

  template <typename T>
  static SharedMemory open(const T &key, std::error_code &ec) {
    return open(std::to_string(key), ec);
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
  typedef void *native_handle;
#elif defined(__linux__)
  typedef int native_handle;

  /// unlink need it
  std::string key_;
#endif // __linux__

  native_handle shmid_;
  void *memory_;
  size_t size_;

}; // class SharedMemory

} // namespace ipc

#endif // IPC_SHAREDMEMORY_H
