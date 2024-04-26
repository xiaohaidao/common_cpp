
#include "gtest/gtest.h"

#include "utils/log.h"

TEST(LogTest, log) {
  LOG_DEBUG("log debug int %d, double %f, str %s .", 1, 1.0, "1");
  LOG_INFO("log info int %d, double %f, str %s .", 1, 1.0, "1");
  LOG_WARN("log warn int %d, double %f, str %s .", 1, 1.0, "1");
  LOG_ERROR("log error int %d, double %f, str %s .", 1, 1.0, "1");

  LOG_DEBUG("log debug int %d, double %f, str %s .\n", 1, 1.0, "1");
  LOG_INFO("log info int %d, double %f, str %s .\n", 1, 1.0, "1");
  LOG_WARN("log warn int %d, double %f, str %s .\n", 1, 1.0, "1");
  LOG_ERROR("log error int %d, double %f, str %s .\n", 1, 1.0, "1");

  constexpr char kBuff1024Char[] =
      "ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"
      "ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"
      "ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"
      "ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"
      "ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"
      "ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"
      "ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"
      "ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"
      "ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"
      "ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"
      "ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"
      "ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"
      "ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"
      "ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"
      "ffffffffffffffff";
  constexpr char kBuff915Char[] =
      "ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"
      "ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"
      "ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"
      "ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"
      "ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"
      "ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"
      "ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"
      "ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"
      "ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"
      "ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"
      "ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"
      "ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"
      "fffffffffffffffffffffffffffffffffffffffffffff";
  LOG_ERROR("log error int %d, double %f, str %s ."
            "%s"
            ".....",
            1, 1.0, "1", kBuff1024Char);
  LOG_ERROR("%s", kBuff915Char);

  set_skip_filename(1);
  LOG_DEBUG("log debug int %d, double %f, str %s .", 1, 1.0, "1");
  LOG_INFO("log info int %d, double %f, str %s .", 1, 1.0, "1");
  LOG_WARN("log warn int %d, double %f, str %s .", 1, 1.0, "1");
  LOG_ERROR("log error int %d, double %f, str %s .", 1, 1.0, "1");
  set_skip_funname(0);
  LOG_DEBUG("log debug int %d, double %f, str %s .", 1, 1.0, "1");
  LOG_INFO("log info int %d, double %f, str %s .", 1, 1.0, "1");
  LOG_WARN("log warn int %d, double %f, str %s .", 1, 1.0, "1");
  LOG_ERROR("log error int %d, double %f, str %s .", 1, 1.0, "1");
  set_skip_filename(0);
  LOG_DEBUG("log debug int %d, double %f, str %s .", 1, 1.0, "1");
  LOG_INFO("log info int %d, double %f, str %s .", 1, 1.0, "1");
  LOG_WARN("log warn int %d, double %f, str %s .", 1, 1.0, "1");
  LOG_ERROR("log error int %d, double %f, str %s .", 1, 1.0, "1");
}
