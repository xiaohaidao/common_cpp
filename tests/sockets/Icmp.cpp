
#include "gtest/gtest.h"

#include <chrono>

#include "sockets/IcmpSocket.h"
#include "sockets/SocketAddr.h"
#include "utils/log.h"

TEST(SocketsTest, IcmpSocketTest) {
  using namespace std::chrono;
  std::error_code ec;
  IcmpSocket server = IcmpSocket::create(kIpV4, ec);
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();

  server.set_read_timeout(2000, ec);
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();

  SocketAddr const addr("www.baidu.com", "8981");
  LOG_DEBUG("ping ip %s", addr.get_ip());

  auto now = std::chrono::steady_clock::now();
  auto last_now = std::chrono::steady_clock::now();
  char buff[128];
  const char data[] = "aabbccddeeffgghh";
  server.send_to(buff, data, sizeof(data), addr, ec);
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();

  std::pair<size_t, SocketAddr> rev = server.recv_from(buff, sizeof(buff), ec);
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();
  now = std::chrono::steady_clock::now();
  LOG_DEBUG("recv time %dus",
            duration_cast<microseconds>(now - last_now).count());

  LOG_DEBUG("client recv from %s:%d message size %d : %s", rev.second.get_ip(),
            rev.second.get_port(), rev.first, buff);

  last_now = std::chrono::steady_clock::now();
  server.send_to(buff, data, sizeof(data), addr, ec);
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();

  rev = server.recv_from(buff, sizeof(buff), ec);
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();
  now = std::chrono::steady_clock::now();
  LOG_DEBUG("recv time %dus",
            duration_cast<microseconds>(now - last_now).count());

  LOG_DEBUG("client recv from %s:%d message size %d : %s", rev.second.get_ip(),
            rev.second.get_port(), rev.first, buff);

  server.close(ec);
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();
}
