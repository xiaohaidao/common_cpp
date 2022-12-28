
#include "gtest/gtest.h"

#include "ipc/Semaphores.h"
#include "utils/log.h"

TEST(IPCTest, SemaphoresTest) {
  std::error_code ec;
  ipc::Semaphores::create(3, ec);
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();

  ipc::Semaphores::create(3, ec);
  EXPECT_TRUE(ec) << ec.value() << " : " << ec.message();
  ec.clear();

  ipc::Semaphores::create("sem_var", ec);
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();

  auto sem1 = ipc::Semaphores::open(3, ec);
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();

  auto sem2 = ipc::Semaphores::open("sem_var", ec);
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();

  sem1.wait(ec);
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();

  sem1.notifyOne(ec);
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();

  sem1.tryWait(ec);
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();
  sem1.tryWaitFor(5, ec);
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();

  sem1.remove(ec);
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();
  sem1.remove(ec); // windows do nothing
  // LOG_TRACE("EXPECT_TRUE %d : %s", ec.value(), ec.message().c_str());
  ec.clear();
  sem2.remove(ec);
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();
}
