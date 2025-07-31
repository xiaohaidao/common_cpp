// Copyright (C) 2022 All rights reserved.
// Email: oxox0@qq.com. Created in 202207

#include "ipc/SharedMemory.h"

namespace ipc {

shared_memory::shared_memory() = default;

void *shared_memory::memory() const { return memory_; }

// size_t shared_memory::size() const { return size_; }

} // namespace ipc
