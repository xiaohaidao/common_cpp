// Copyright (C) 2022 dobot inc All rights reserved.
// Author: libobo@dobot.cc. Created in 202204

#include "gtest/gtest.h"

#include "min_heap/max_heap.h"
#include "min_heap/min_heap.h"

TEST(HeapTest, min_heap) {
  min_heap<int> h;
  h.push(8);
  h.push(6);
  h.push(-8);
  h.push(0);
  h.push(2);
  EXPECT_EQ(*h.front(), -8);
  // h.dump(std::cout);
  h.pop();
  EXPECT_EQ(*h.front(), 0);
  h.pop();
  EXPECT_EQ(*h.front(), 2);
  h.pop();
  EXPECT_EQ(*h.front(), 6);
  h.pop();
  EXPECT_EQ(*h.front(), 8);
  h.pop();
}

TEST(HeapTest, max_heap) {
  max_heap<int> h;
  h.push(8);
  h.push(6);
  h.push(-8);
  h.push(0);
  h.push(2);
  EXPECT_EQ(*h.front(), 8);
  h.pop();
  EXPECT_EQ(*h.front(), 6);
  h.pop();
  EXPECT_EQ(*h.front(), 2);
  h.pop();
  EXPECT_EQ(*h.front(), 0);
  h.pop();
  EXPECT_EQ(*h.front(), -8);
  h.pop();
}
