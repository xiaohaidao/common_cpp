// Copyright (C) 2022 All rights reserved.
// Email: oxox0@qq.com. Created in 202207

#include "ipc/SharedMemory.h"

namespace ipc {

SharedMemory::SharedMemory() : shmid_(0), memory_(nullptr), size_(0) {}

void *SharedMemory::memory() const { return memory_; }

size_t SharedMemory::size() const { return size_; }

} // namespace ipc
