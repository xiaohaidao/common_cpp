#include <set>
#include <thread>

#include "lock_free/lock_free_stack.h"
#include "gtest/gtest.h"

void thread_func(thread::lock_free_stack<int> &stack, size_t id) {
  static const int kLoopSize = 1000;
  for (size_t i = 0; i < kLoopSize; ++i) {
    int const v = (int)((id + 1) * kLoopSize + i);
    stack.push(v);
  }
}

TEST(LockFreeStack, LockFreeStackFunction1) {

  thread::lock_free_stack<int> stack;

  size_t const thread_size = std::thread::hardware_concurrency();
  std::vector<std::thread> threads;
  for (size_t i = 0; i < thread_size; ++i) {
    threads.push_back(std::thread(thread_func, std::ref(stack), i));
  }

  for (size_t i = 0; i < thread_size; ++i) {
    threads[i].join();
  }

  std::set<int> result;
  while (auto v = stack.pop()) {
    ASSERT_TRUE(result.find(*v) == result.end());
    result.insert(*v);
  }
}
