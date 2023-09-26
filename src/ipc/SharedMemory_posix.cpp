// Copyright (C) 2022 All rights reserved.
// Email: oxox0@qq.com. Created in 202210

/**
 * @file
 *
 *
 * @brief shared memory of posix
 *
 * 参看：https://man7.org/linux/man-pages/man7/shm_overview.7.html
 *
 */

#ifdef __linux__

#include "ipc/SharedMemory.h"

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#include "utils/error_code.h"
#include "utils/macro.h"

namespace ipc {

SharedMemory::~SharedMemory() {}

SharedMemory SharedMemory::open(const char *key, std::error_code &ec) {
  SharedMemory result;
  if ((result.shmid_ = shm_open(key, O_RDWR, 0)) == -1) {
    ec = getErrorCode();
    return result;
  }
  result.key_ = key;

  struct stat statbuff = {};
  if (fstat(result.shmid_, &statbuff) == -1) {
    ec = std::error_code(errno, std::system_category());
    return result;
  }
  result.size_ = statbuff.st_size;
  result.attach(ec);

  return result;
}

SharedMemory SharedMemory::create(const char *key, size_t mem_size,
                                  std::error_code &ec) {
  SharedMemory result;
  //  delete O_EXCL will create force
  if ((result.shmid_ = shm_open(key, O_CREAT | O_EXCL | O_RDWR, DEFFILEMODE)) ==
      -1) {

    ec = getErrorCode();
    return result;
  }
  result.key_ = key;

  size_t size = mem_size;
  if (ftruncate(result.shmid_, size)) {
    ec = getErrorCode();
    return result;
  }
  result.size_ = size;
  result.attach(ec);

  return result;
}

void SharedMemory::deatch(std::error_code &ec) {
  if (memory_ == nullptr) {
    return;
  }
  if (munmap(memory_, size_) == -1) {
    ec = getErrorCode();
    return;
  }

  memory_ = nullptr;
  size_ = 0;
}

void SharedMemory::attach(std::error_code &ec) {
  if ((memory_ = mmap(nullptr, size_, PROT_READ | PROT_WRITE, MAP_SHARED,
                      shmid_, 0)) == MAP_FAILED) {
    ec = getErrorCode();
    memory_ = nullptr;
    size_ = 0;
  }
}

void SharedMemory::close(std::error_code &ec) {
  deatch(ec);
  if (shmid_ <= 0) {
    return;
  }
  if (::close(shmid_) == -1) {
    ec = getErrorCode();
    return;
  }
  shmid_ = 0;
}

void SharedMemory::remove(std::error_code &ec) {
  close(ec);
  if (shm_unlink(key_.c_str()) == -1) {
    ec = getErrorCode();
    return;
  }
}

} // namespace ipc

#endif // __linux__
