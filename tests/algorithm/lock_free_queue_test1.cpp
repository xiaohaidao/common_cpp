#include <set>
#include <thread>

#include "lock_free/interruptible_thread.h"
#include "lock_free/lock_free_queue.h"
#include "gtest/gtest.h"

void thread_func(thread::lock_free_queue<int> &queue, size_t id) {
  static const int kLoopSize = 1000;
  for (size_t i = 0; i < kLoopSize; ++i) {
    int const v = (int)((id + 1) * kLoopSize + i);
    queue.push(v);
  }
}

TEST(LockFreeQueue, LockFreeQueueFunction1) {
  thread::lock_free_queue<int> queue;

  size_t const thread_size = std::thread::hardware_concurrency();
  std::vector<thread::interruptible_thread> threads;
  for (size_t i = 0; i < thread_size; ++i) {
    threads.push_back(thread::interruptible_thread(
        std::bind(thread_func, std::ref(queue), i)));
  }

  for (size_t i = 0; i < thread_size; ++i) {
    threads[i].join();
  }

  std::set<int> result;
  while (auto v = queue.pop()) {
    ASSERT_TRUE(result.find(*v) == result.end());
    result.insert(*v);
  }
}
