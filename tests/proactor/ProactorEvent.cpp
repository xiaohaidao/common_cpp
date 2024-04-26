
#include "gtest/gtest.h"

#include "proactor/Proactor.h"
#include "proactor/operation/EventOp.h"
#include "utils/log.h"

namespace proactor_eventop {

TEST(ProactorTest, Eventop) {
  std::error_code ec;
  Proactor p(ec);
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();

  EventOp event(&p);

  event.async_notify([](const std::error_code & /*re*/,
                        size_t /*size*/) { LOG_DEBUG("notify done"); },
                     ec);
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();

  LOG_DEBUG("-------------------- begin run while --------------------");
  for (size_t i = 0; i < 10; ++i) {
    p.run_one(1000ull * 1000ull, ec);
    EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
    ec.clear();
    event.async_notify([i](const std::error_code & /*re*/,
                           size_t /*size*/) { LOG_DEBUG("notify done %d", i); },
                       ec);
  }
  LOG_DEBUG("-------------------- end run while --------------------");

  p.close(ec);
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();
}

} // namespace proactor_eventop
