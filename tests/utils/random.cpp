// Copyright (C) 2022 dobot inc All rights reserved.
// Author: libobo@dobot.cc. Created in 202204

#include "gtest/gtest.h"

#include "utils/random.h"

TEST(RandomTest, randomScopeTest) {
  for (size_t i = 0; i < 100; ++i) {
    auto v = randScope(899, 1099);
    EXPECT_TRUE(v >= 899);
    EXPECT_TRUE(v <= 1099);
  }

  for (size_t i = 0; i < 100; ++i) {
    auto v = randScope(1099, 899);
    EXPECT_TRUE(v >= 899);
    EXPECT_TRUE(v <= 1099);
  }

  for (size_t i = 0; i < 100; ++i) {
    auto v = randScope(899, 899);
    EXPECT_TRUE(v >= 899);
    EXPECT_TRUE(v <= 899);
  }
  for (size_t i = 0; i < 100; ++i) {
    auto v = randScope(899, 900);
    EXPECT_TRUE(v >= 899);
    EXPECT_TRUE(v <= 900);
  }
  for (size_t i = 0; i < 100; ++i) {
    auto v = randScope(899, 901);
    EXPECT_TRUE(v >= 899);
    EXPECT_TRUE(v <= 901);
  }
  for (size_t i = 0; i < 100; ++i) {
    auto v = randScope(899, 902);
    EXPECT_TRUE(v >= 899);
    EXPECT_TRUE(v <= 902);
  }
}

TEST(RandomTest, randomNumTest) {
  std::set<uint64_t> exist;
  for (size_t i = 0; i < 10000; ++i) {
    auto v = randNum();
    EXPECT_TRUE(exist.find(v) == exist.end());
    exist.insert(v);
  }

  fixSeed(); // fix seed 15011051792192176828u
  size_t mem[] = {
      3768849550270110920u,  3768849550270122440u,  5288703135886362264u,
      5003422398985123289u,  4543665650187777649u,  4382799437429908962u,
      15205833326636946245u, 2679467723279289907u,  9627054335744294083u,
      686932134009166551u,   18328302966415786447u, 15991823240820792052u,
      2273620603928325263u,  6536478448155270017u,  1833753642079743296u,
      3618716850736863959u,  15043758070891452689u, 13653306181875854466u,
      582806529032327977u,   5780455872252895938u};
  for (size_t i = 0; i < 20; ++i) {
    EXPECT_TRUE(mem[i] == randNum());
  }
}
