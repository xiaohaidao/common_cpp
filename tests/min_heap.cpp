// Copyright (C) 2022 dobot inc All rights reserved.
// Author: libobo@dobot.cc. Created in 202204

#include "gtest/gtest.h"

#include <chrono>

#include "min_heap/max_heap.h"
#include "min_heap/min_heap.h"
#include "min_heap/timer_task.h"

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

TEST(HeapTest, min_heap_time) {
  auto now = std::chrono::steady_clock::now();
  auto now1 = now + std::chrono::milliseconds(109);
  auto now2 = now + std::chrono::milliseconds(108);
  auto now3 = now + std::chrono::milliseconds(110);
  auto now4 = now + std::chrono::milliseconds(-10);
  min_heap<std::chrono::time_point<std::chrono::steady_clock> > h;
  h.push(now);
  h.push(now1);
  h.push(now2);
  h.push(now3);
  h.push(now4);
  h.erase(now);
  // std::cout << "{";
  // bool first = true;
  // while (!h.empty()) {
  //   if (!first)
  //     std::cout << ", ";
  //   std::cout << h.front()->time_since_epoch().count() << "ns";
  //   first = false;
  //   h.pop();
  // }
  // std::cout << "}\n";
  // std::cout << decltype(now)::period::num << "/" << decltype(now)::period::den
  //           << "\n";
  EXPECT_EQ(now4, *h.front());
  h.pop();
  EXPECT_EQ(*h.front(), now2);
  h.pop();
  EXPECT_EQ(*h.front(), now1);
  h.pop();
  EXPECT_EQ(*h.front(), now3);

  // test empty
  h.pop();
}

class myKey {
public:
  myKey() : k_(0) {}
  myKey(uint32_t k) : k_(k) {}
  ~myKey() {}
  bool operator<(const myKey &r) const { return this->k_ < r.k_; }

private:
  uint32_t k_;
};

TEST(HeapTest, timer_task) {
  timer_task<myKey> task;
  auto p = []() {
    std::cout << "time now :"
              << std::chrono::duration_cast<std::chrono::milliseconds>(
                     std::chrono::system_clock::now().time_since_epoch())
                     .count()
              << "\n";
  };

  task.post(1, 1000, p);
  task.post(2, 2000, p);
  task.post(3, 3000, p);
  task.post(4, 4000, p);
  task.post(2, 4000, p);
  // while (!task.empty());
}