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

  // 1 block size is 4k
  static SharedMemory create(const std::string &key, size_t block_num,
                             std::error_code &ec);

  template <typename T>
  static SharedMemory create(const T &key, size_t block_num,
                             std::error_code &ec) {
    return create(std::to_string(key), block_num, ec);
  }

  static SharedMemory get(const std::string &key, std::error_code &ec);

  template <typename T>
  static SharedMemory get(const T &key, std::error_code &ec) {
    return get(std::to_string(key), ec);
  }

  static constexpr size_t blockSize() { return 4096; }

  void *memory() const;
  size_t size() const;

  void del(std::error_code &ec);

private:
  void attach(std::error_code &ec);

  void deatch(std::error_code &ec);

  int shmid_;
  void *memory_;
  size_t size_;

  /// shm_unlink need it
  std::string key_;

}; // class SharedMemory

} // namespace ipc

#endif // IPC_SHAREDMEMORY_H
