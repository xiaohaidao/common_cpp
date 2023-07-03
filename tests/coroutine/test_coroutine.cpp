
#include "gtest/gtest.h"

#include <chrono>
#include <thread>

#include "coroutine/coroutine.h"
#include "utils/log.h"

static void func1(coroutine &co) {
  LOG_TRACE("func1: started\n");
  LOG_TRACE("func1: swapcontext next\n");
  co.yield();
  LOG_TRACE("func1: returning\n");
}

static void func2(void) {
  LOG_TRACE("func2: started\n");
  LOG_TRACE("func2: swapcontext next\n");
  co_yield();
  LOG_TRACE("func2: returning\n");
}

static void func4(void) {
  LOG_TRACE("func4: started\n");
  LOG_TRACE("func4: swapcontext next\n");
  co_yield();
  LOG_TRACE("func4: returning\n");
}

static void func3(void) {
  LOG_TRACE("func3: started\n");
  func4();
  LOG_TRACE("func3: swapcontext next\n");
  co_yield();
  LOG_TRACE("func3: returning\n");
}

void co_call() {
  LOG_TRACE("task begin\n");
  // coroutine co;
  co_await(func1);
  co_await(func2);
  co_await(func3);

  LOG_TRACE("main task\n");
  co_yield();
  LOG_TRACE("main run yield\n");
  co_yield();

  LOG_TRACE("main append task again\n");
  co_await(func2);
  co_await(func3);
  co_await(func1);
  co_loop();
  LOG_TRACE("main task done\n");
}

TEST(Coroutine, coroutine) {
  LOG_TRACE("test task begin\n");

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
  LOG_TRACE("test task begin end \n");
}
