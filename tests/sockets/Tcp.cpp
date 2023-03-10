
#include "gtest/gtest.h"

#include "sockets/SocketAddr.h"
#include "sockets/TcpListener.h"
#include "sockets/TcpStream.h"
#include "utils/log.h"

TEST(SocketsTest, TcpTest) {
  // server
  std::error_code ec;

  SocketAddr addr("127.0.0.1", "8988");
  LOG_TRACE("local ip is %s port %d", addr.get_ip(), addr.get_port());

  auto tcp = TcpListener::bind(std::to_string(addr.get_port()).c_str(), ec);
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();
  EXPECT_TRUE(tcp.native_handle() > 0);
  LOG_TRACE("server socket value %d, default read timeout %d",
            tcp.native_handle(), tcp.read_timeout(ec));
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();

  tcp.set_read_timeout(100, ec);
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();
  LOG_TRACE("server socket read timeout %d", tcp.read_timeout(ec));
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();

  // client
  auto st = TcpStream::connect(addr, ec);
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();
  EXPECT_TRUE(st.native_handle() > 0);
  LOG_TRACE("clinet socket value %d", st.native_handle());

  st.set_read_timeout(100, ec);
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();
  st.set_write_timeout(100, ec);
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();
  LOG_TRACE("client read timeout %dms and write timeout %dms",
            st.read_timeout(ec), st.write_timeout(ec));
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();

  char buff[] = "send message!";
  memset(buff, 0, sizeof(buff));
  size_t s = st.read(buff, sizeof(buff), ec);
  // EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();
  LOG_TRACE("read buff %d: %s", s, buff);
  // if (s == 0 && !ec) {
  //   st.close(ec);
  //   EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  //   ec.clear();
  // }

  st.write(buff, strlen(buff), ec);
  // EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();

  st.close(ec);
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();

  tcp.close(ec);
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();
}
