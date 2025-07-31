
#include "gtest/gtest.h"

#include "ipc/Semaphores.h"
#include "utils/log.h"

TEST(IPCTest, SemaphoresTest) {
  std::error_code ec;
  ipc::semaphores::create(3, ec);
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();

  ipc::semaphores::create(3, ec);
  EXPECT_TRUE(ec) << ec.value() << " : " << ec.message();
  ec.clear();

  ipc::semaphores::create("sem_var", ec);
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();

  auto sem1 = ipc::semaphores::open(3, ec);
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();

  auto sem2 = ipc::semaphores::open("sem_var", ec);
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();

  sem1.wait(ec);
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();

  sem1.notify_one(ec);
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();

  sem1.try_wait(ec);
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();
  sem1.try_wait_for(5, ec);
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();

  sem1.remove(ec);
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();
  sem1.remove(ec); // windows do nothing
  // LOG_DEBUG("EXPECT_TRUE %d : %s", ec.value(), ec.message().c_str());
  ec.clear();
  sem2.remove(ec);
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();
}
