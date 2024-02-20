
#include "gtest/gtest.h"

#include <array>

#include "proactor/Proactor.h"
#include "proactor/operation/TimerOp.h"
#include "utils/log.h"

using namespace std::placeholders; // for _1, _2, _3...

TEST(ProactorTest, ProactorTimer) {
  std::error_code ec;
  Proactor p(ec);
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();

  TimerOp timer(p);
  auto call_back = [](const char *b, const std::error_code &ec, size_t n) {
    LOG_TRACE("%s timer async call n %u!", b, n);
    if (ec) {
      LOG_TRACE("error %s", ec.message().c_str());
    }
  };
  timer.set_timeout(600);
  timer.async_wait(std::bind(call_back, "timer interval 0", _1, _2), ec);
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();

  TimerOp timer1(p);
  timer1.set_timeout(200, 300);
  timer1.async_wait(std::bind(call_back, "timer1 interval 300", _1, _2), ec);
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();

  TimerOp timer2(p);
  timer2.set_timeout(100, 100);
  timer2.async_wait(std::bind(call_back, "timer2 interval 100", _1, _2), ec);
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();

  timer1.wait();

  TimerOp timer3(p);
  timer3.set_timeout(100, 300);

  LOG_TRACE("-------------------- begin run while --------------------");
  for (size_t i = 0; i < 20; ++i) {
    p.run_one(1000 * 1000, ec);
    EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
    ec.clear();
    if (i == 3) {
      timer1.close(ec);
      EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
      ec.clear();

      timer3.async_wait(std::bind(call_back, "timer3 interval 300", _1, _2),
                        ec);
      EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
      ec.clear();
    }
  }
  LOG_TRACE("-------------------- end run while --------------------");

  timer.close(ec);
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();

  timer1.close(ec);
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();
}
