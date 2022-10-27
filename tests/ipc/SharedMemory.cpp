// Copyright (C) 2022 dobot inc All rights reserved.
// Author: libobo@dobot.cc. Created in 202204

#include "gtest/gtest.h"

#include "ipc/SharedMemory.h"
#include "utils/log.h"

TEST(IPCTest, SharedMemoryTest) {
  std::error_code ec;
  ipc::SharedMemory::create(3, 20, ec);
  EXPECT_FALSE(ec) << ec.message();
  ec.clear();
  ipc::SharedMemory::create(1, 2, ec);
  EXPECT_FALSE(ec) << ec.message();
  ec.clear();
  ipc::SharedMemory::create(3, 30, ec);
  EXPECT_TRUE(ec) << ec.message();
  ec.clear();

  auto sh1 = ipc::SharedMemory::get(3, ec);
  EXPECT_FALSE(ec) << ec.message();
  ec.clear();
  ipc::SharedMemory::get(2, ec);
  EXPECT_TRUE(ec) << ec.message();
  ec.clear();

  auto sh2 = ipc::SharedMemory::get(1, ec);
  EXPECT_FALSE(ec) << ec.message();
  ec.clear();

  sh1.del(ec);
  EXPECT_FALSE(ec) << ec.message();
  sh2.del(ec);
  EXPECT_FALSE(ec) << ec.message();
}
