
#include "gtest/gtest.h"

#include "proactor/Proactor.h"
#include "utils/log.h"

TEST(ProactorTest, Proactor) {
  using namespace sockets;
  std::error_code ec;
  auto local = SocketAddr::getLocalAddress(ec);
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
}
