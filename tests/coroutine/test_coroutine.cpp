
#include "gtest/gtest.h"

#include <chrono>

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
  LOG_TRACE("func2: returning\n");
}

static void func3(void) {
  LOG_TRACE("func3: started\n");
  LOG_TRACE("func3: returning\n");
}

TEST(Coroutine, coroutine) {
  coroutine co;
  co.append_task(func1);
  co.append_task(func2);
  co.append_task(func3);

  LOG_TRACE("main task\n");
  co.yield();
  LOG_TRACE("main run yield\n");
  co.yield();

  LOG_TRACE("main append task again\n");
  co.append_task(func2);
  co.append_task(func3);
  co.append_task(func1);
  LOG_TRACE("main task done\n");
}
