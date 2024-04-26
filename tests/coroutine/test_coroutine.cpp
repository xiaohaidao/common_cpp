
#include "gtest/gtest.h"

#include <chrono>
#include <thread>

#include "coroutine/coroutine.h"
#include "utils/log.h"

namespace {

void func1(coroutine &co) {
  LOG_DEBUG("func1: started\n");
  LOG_DEBUG("func1: swapcontext next\n");
  co.yield();
  LOG_DEBUG("func1: returning\n");
}

void func2(void) {
  LOG_DEBUG("func2: started\n");
  LOG_DEBUG("func2: swapcontext next\n");
  co_yield();
  LOG_DEBUG("func2: returning\n");
}

void func4(void) {
  LOG_DEBUG("func4: started\n");
  LOG_DEBUG("func4: swapcontext next\n");
  co_yield();
  LOG_DEBUG("func4: returning\n");
}

void func3(void) {
  LOG_DEBUG("func3: started\n");
  func4();
  LOG_DEBUG("func3: swapcontext next\n");
  co_yield();
  LOG_DEBUG("func3: returning\n");
}

} // namespace

void co_call() {
  LOG_DEBUG("task begin\n");
  // coroutine co;
  co_await(func1);
  co_await(func2);
  co_await(func3);

  LOG_DEBUG("main task\n");
  co_yield();
  LOG_DEBUG("main run yield\n");
  co_yield();

  LOG_DEBUG("main append task again\n");
  co_await(func2);
  co_await(func3);
  co_await(func1);
  co_loop();
  LOG_DEBUG("main task done\n");
}

TEST(Coroutine, coroutine) {
  LOG_DEBUG("test task begin\n");

  co_call();

  std::thread t1(co_call);
  std::thread t2(co_call);
  std::thread t3(co_call);
  std::thread t4(co_call);
  t1.join();
  t2.join();
  t3.join();
  t4.join();

  co_call();
  LOG_DEBUG("test task begin end \n");
}
