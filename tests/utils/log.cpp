// Copyright (C) 2022 dobot inc All rights reserved.
// Author: libobo@dobot.cc. Created in 202204

#include "gtest/gtest.h"

#include "utils/log.h"

TEST(LogTest, log) {
  LOG_TRACE("log trace int %d, double %f, str %s .", 1, 1.0, "1");
  LOG_DEBUG("log debug int %d, double %f, str %s .", 1, 1.0, "1");
  LOG_INFO("log info int %d, double %f, str %s .", 1, 1.0, "1");
  LOG_WARN("log warn int %d, double %f, str %s .", 1, 1.0, "1");
  LOG_ERROR("log error int %d, double %f, str %s .", 1, 1.0, "1");
}
