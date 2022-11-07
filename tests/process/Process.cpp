// Copyright (C) 2022 dobot inc All rights reserved.
// Author: libobo@dobot.cc. Created in 202204

#include "gtest/gtest.h"

#include "process/Process.h"
#include "utils/log.h"

TEST(ProcessTest, Process) {
  std::error_code ec;
#ifdef WIN32
  auto child =
      Process::call("c:/windows/system32/cmd.exe", {"/c", "net user"}, ec);
  // auto child = Process::call("c:/windows/notepad.exe", {}, ec);
#else
  auto child = Process::call("id", {"id"}, ec);
  // auto child = Process::call("sleep", {"sleep", "50"}, ec);
  // auto child = Process::call("/bin/sh", {"sh", "-c", "ls -al"}, ec);
  // auto child = Process::call("/bin/sh", {"sh", "-c", "id"}, ec);
#endif // WIN32
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();

  EXPECT_TRUE(child.running(ec));
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();

  LOG_TRACE("wait begin");
  child.wait(ec);
  LOG_TRACE("wait end");
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();

  child.wait(ec);
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();

  EXPECT_FALSE(child.running(ec));
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();

  Process p = Process::open(115200, ec); // empty pid
  // LOG_TRACE("EXPECT_TRUE %d : %s", ec.value(), ec.message().c_str());
  ec.clear();
  EXPECT_FALSE(p.running(ec));
  // LOG_TRACE("EXPECT_FALSE %d : %s", ec.value(), ec.message().c_str());
  ec.clear();

  p.terminate(ec);
  // LOG_TRACE("EXPECT_TRUE %d : %s", ec.value(), ec.message().c_str());
  ec.clear();

  p.wait(ec);
  // LOG_TRACE("EXPECT_FALSE %d : %s", ec.value(), ec.message().c_str());
  ec.clear();
}
