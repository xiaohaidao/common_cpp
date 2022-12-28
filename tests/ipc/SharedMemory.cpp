
#include "gtest/gtest.h"

#include "ipc/SharedMemory.h"
#include "utils/log.h"

TEST(IPCTest, SharedMemoryTest) {
  std::error_code ec;
  ipc::SharedMemory::create(33333, 200, ec);
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();
  ipc::SharedMemory::create("sh_momery", 200, ec);
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();
  ipc::SharedMemory::create(33333, 30, ec);
  EXPECT_TRUE(ec) << ec.value() << " : " << ec.message();
  ec.clear();

  auto sh1 = ipc::SharedMemory::open(33333, ec);
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();
  ipc::SharedMemory::open(2, ec);
  EXPECT_TRUE(ec) << ec.value() << " : " << ec.message();
  ec.clear();

  auto sh2 = ipc::SharedMemory::open("sh_momery", ec);
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();

  sh1.remove(ec);
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();
  sh1.remove(ec); // windows do nothing
  // LOG_TRACE("EXPECT_TRUE %d : %s", ec.value(), ec.message().c_str());
  ec.clear();
  sh2.remove(ec);
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();
}
