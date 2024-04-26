
#include "gtest/gtest.h"

#include "process/Process.h"
#include "utils/log.h"

TEST(ProcessTest, Process) {
  std::error_code ec;
#ifdef _WIN32
  auto child = Process::call("cmd.exe", {"/c", "net user"}, ec);
  // auto child = Process::call("c:/windows/notepad.exe", {}, ec);
#else
  auto child = Process::call("id", {}, ec);
  // auto child = Process::call("sleep", {"50"}, ec);
  // auto child = Process::call("/bin/sh", {"-c", "ls -al"}, ec);
  // auto child = Process::call("/bin/sh", {"-c", "id"}, ec);
#endif // _WIN32
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();

  EXPECT_TRUE(child.running(ec));
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();

  LOG_DEBUG("wait begin");
  child.wait(ec);
  LOG_DEBUG("wait end");
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();

  child.wait(ec);
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();

  EXPECT_FALSE(child.running(ec));
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();

  Process p = Process::open(115200, ec); // empty pid
  // LOG_DEBUG("EXPECT_TRUE %d : %s", ec.value(), ec.message().c_str());
  ec.clear();
  EXPECT_FALSE(p.running(ec));
  // LOG_DEBUG("EXPECT_FALSE %d : %s", ec.value(), ec.message().c_str());
  ec.clear();

  p.terminate(ec);
  // LOG_DEBUG("EXPECT_TRUE %d : %s", ec.value(), ec.message().c_str());
  ec.clear();

  p.wait(ec);
  // LOG_DEBUG("EXPECT_FALSE %d : %s", ec.value(), ec.message().c_str());
  ec.clear();
}
