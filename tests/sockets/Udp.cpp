
#include "gtest/gtest.h"

#include "sockets/SocketAddr.h"
#include "sockets/UdpSocket.h"
#include "utils/log.h"

TEST(SocketsTest, UdpSocketTest) {
  std::error_code ec;
  UdpSocket server = UdpSocket::create(kIpV4, ec);
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();
  LOG_TRACE("server udp default value: loop %d ttl %d ",
            server.multicast_loop(ec), server.multicast_ttl(ec));
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();
  server.set_multicast_loop(true, ec);
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();

  // braodcast
  LOG_TRACE("server braodcast enable %d", server.broadcast(ec));
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();

  LOG_TRACE("set server braodcast enable 1");
  server.set_broadcast(true, ec);
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();

  LOG_TRACE("server braodcast enable %d", server.broadcast(ec));
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();

  SocketAddr addr("224.0.0.5", "8981");
  LOG_TRACE("multicast ip is %s port %d", addr.get_ip(), addr.get_port());

  // initialize client
  UdpSocket client = UdpSocket::bind("8981", ec);
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();

  SocketAddr local_addr =
      SocketAddr::get_local_socket(client.native_handle(), ec);
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();
  LOG_TRACE("clinet socket value %d  %s:%d", client.native_handle(),
            local_addr.get_ip(), local_addr.get_port());

  client.connected({"www.baidu.com", "80"}, ec);
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();
  local_addr = SocketAddr::get_local_socket(client.native_handle(), ec);
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();
  LOG_TRACE("clinet socket value %d  %s:%d", client.native_handle(),
            local_addr.get_ip(), local_addr.get_port());

  client.set_multicast_loop(true, ec);
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();
  client.set_multicast_ttl(10, ec);
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();
  client.joint_multicast(addr, ec);
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();
  LOG_TRACE("client udp value: loop %d ttl %d ",
            (uint8_t)client.multicast_loop(ec), client.multicast_ttl(ec));
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();

  char buff[] = "multicast message!";
  server.send_to(buff, sizeof(buff), addr, ec);
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();
  LOG_TRACE("send message to multicast!");
  memset(buff, 0, sizeof(buff));

  client.set_read_timeout(100, ec);
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();
  std::pair<size_t, SocketAddr> rev = client.recv_from(buff, sizeof(buff), ec);
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();

  LOG_TRACE("client recv from %s:%d message size %d : %s", rev.second.get_ip(),
            rev.second.get_port(), rev.first, buff);

  client.leave_multicast(addr, ec);
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();

  server.close(ec);
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();
  client.close(ec);
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();
}
