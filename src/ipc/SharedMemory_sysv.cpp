// Copyright (C) 2022 All rights reserved.
// Email: oxox0@qq.com. Created in 202207

/**
 * @file
 *
 *
 * @brief shared memory of system v
 *
 * ipcs -a show the all system information
 * ipcrm remove the system struct
 *
 */

#ifdef __linux__

#include "ipc/SharedMemory.h"

#include <sys/shm.h>

#include "utils/error_code.h"

namespace ipc {

shared_memory shared_memory::open(const std::string &key, std::error_code &ec) {
  shared_memory result;
  result.shmid_ = shmget(ftok(key.c_str(), 0), 0, 0);
  if (result.shmid_ == -1) {
    ec = std::error_code(errno, std::system_category());
    return result;
  }

  result.attach(ec);
  return result;
}

shared_memory shared_memory::create(const std::string &key, size_t block_num,
                                    std::error_code &ec) {
  shared_memory result;
  result.shmid_ = shmget(ftok(key.c_str(), 0), block_num * 4096,
                         0666 | IPC_CREAT | IPC_EXCL);
  if (result.shmid_ == -1) {
    ec = get_error_code();
    // return result;
  }

  result.attach(ec);
  return result;
}

void shared_memory::deatch(std::error_code &ec) {
  if (shmdt(memory_) == -1) {
    ec = get_error_code();
    return;
  }
  memory_ = nullptr;
}

void shared_memory::attach(std::error_code &ec) {
  deatch(ec);
  if (ec) {
    // return;
  }
  memory_ = shmat(shmid_, 0, 0);
  if (memory_ == (void *)-1) {
    ec = get_error_code();
    memory_ = nullptr;
  }
}

void shared_memory::close(std::error_code &ec) {}

void shared_memory::remove(std::error_code &ec) {
  if (shmctl(shmid_, IPC_RMID, 0) == -1) {
    ec = get_error_code();
    return;
  }
  shmid_ = 0;
  memory_ = nullptr;
}

} // namespace ipc

#endif // __linux__
