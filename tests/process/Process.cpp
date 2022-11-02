// Copyright (C) 2022 dobot inc All rights reserved.
// Author: libobo@dobot.cc. Created in 202204

#include "gtest/gtest.h"

#include "process/Process.h"

TEST(ProcessTest, Process) {
  std::error_code ec;
  char const *const argv[] = {"-c", "gettext.sh", NULL};
  char const *const env[] = {NULL};
  auto child = Process::call("/bin/sh", argv, env, ec);
  EXPECT_FALSE(ec) << ec.message();
  ec.clear();

  EXPECT_TRUE(child.running(ec));
  EXPECT_FALSE(ec) << ec.message();
  ec.clear();

  child.wait(ec);
  EXPECT_FALSE(ec) << ec.message();
  ec.clear();

  child.wait(ec);
  EXPECT_FALSE(ec) << ec.message();
  ec.clear();

  EXPECT_FALSE(child.running(ec));
  EXPECT_FALSE(ec) << ec.message();
  ec.clear();

  Process p(115200); // empty pid
  EXPECT_FALSE(p.running(ec));
  EXPECT_FALSE(ec) << ec.message();
  ec.clear();

  p.terminate(ec);
  EXPECT_TRUE(ec) << ec.message();
  ec.clear();

  p.wait(ec);
  EXPECT_FALSE(ec) << ec.message();
  ec.clear();
}
