
#include "gtest/gtest.h"

#include "sockets/SocketAddr.h"
#include "utils/log.h"

TEST(SocketsTest, SocketAddr) {
  using namespace sockets;
  std::error_code ec;
  auto local = SocketAddr::resolve_host(nullptr, "http", ec, kIpV6);
  // auto local = SocketAddr::resolve_host(nullptr, "80", ec);
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();
  LOG_DEBUG("local ip is %s port %d", local.get_ip(), local.get_port());

  char buff1[64] = {}, buff2[64] = {};
  local.get_nameinfo(buff1, sizeof(buff1), buff2, sizeof(buff2), ec);
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();
  LOG_DEBUG("hostname %s service %s", buff1, buff2);

  const char *host = SocketAddr::get_localhost(ec);
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();
  LOG_DEBUG("localhost name %s", host);

  local = SocketAddr::resolve_host("www.baidu.com", nullptr, ec);
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();
  LOG_DEBUG("www.baidu.com ip is %s port %d", local.get_ip(), local.get_port());
  memset(buff1, 0, sizeof(buff1));
  memset(buff2, 0, sizeof(buff2));
  local.get_nameinfo(buff1, sizeof(buff1), buff2, sizeof(buff2), ec);
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();
  LOG_DEBUG("hostname %s service %s", buff1, buff2);

  auto local_all = SocketAddr::resolve_host_all("www.jd.com", "ftp", ec);
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();
  for (auto &i : local_all) {
    LOG_DEBUG("www.jd.com ip is %s port %d", i.get_ip(), i.get_port());
  }

  auto host_all = SocketAddr::resolve_host_all(host, "ftp", ec);
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();
  for (auto &i : host_all) {
    LOG_DEBUG("host %s ip is %s port %d", host, i.get_ip(), i.get_port());
  }

  local = SocketAddr::resolve_host("192.168.0.1", "http", ec);
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();
  LOG_DEBUG("local ip is %s port %d", local.get_ip(), local.get_port());
  memset(buff1, 0, sizeof(buff1));
  memset(buff2, 0, sizeof(buff2));
  local.get_nameinfo(buff1, sizeof(buff1), buff2, sizeof(buff2), ec);
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();
  LOG_DEBUG("hostname %s service %s", buff1, buff2);

  local.set_port(9999);
  LOG_DEBUG("set local ip is %s port %d", local.get_ip(), local.get_port());

  // get local ip mask
  LOG_DEBUG("get local all ipv4");
  auto local_all_ip = SocketAddr::get_local_ip_mask(ec);
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();
  for (auto &i : local_all_ip) {
    LOG_DEBUG("%u local ip %s, mask %s, broad addr %s",
              *(uint32_t *)(std::get<0>(i).native_ip_addr()),
              std::get<0>(i).get_ip(), std::get<1>(i).get_ip(),
              std::get<2>(i).get_ip());
  }

  LOG_DEBUG("get local all ipv6");
  local_all_ip = SocketAddr::get_local_ip_mask(ec, kIpV6);
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();
  for (auto &i : local_all_ip) {
    LOG_DEBUG("local ip %s, mask %s, broad addr %s", std::get<0>(i).get_ip(),
              std::get<1>(i).get_ip(), std::get<2>(i).get_ip());
  }
}
