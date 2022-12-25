
#include "gtest/gtest.h"

#include "sockets/SocketAddr.h"
#include "sockets/TcpListener.h"
#include "sockets/TcpStream.h"
#include "utils/log.h"

TEST(SocketsTest, SelectTcpBlock) {
  return;
  // server
  using namespace sockets;
  std::error_code ec;

  SocketAddr addr("127.0.0.1", "8981");
  LOG_TRACE("local ip is %s port %d", addr.get_ip(), addr.get_port());

  auto tcp = TcpListener::bind("8980", ec);
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();
  EXPECT_TRUE(tcp.native_handle() > 0);
  LOG_TRACE("server socket value %d", tcp.native_handle());

  auto rec = tcp.accept(ec);
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();

  tcp.close(ec);
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();

  TcpStream &st = rec.first;
  char buff[128] = "server begin send message";
  st.write(buff, strlen(buff), ec);
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();
  LOG_TRACE("client ip and port %s:%d", rec.second.get_ip(),
            rec.second.get_port());

  memset(buff, 0, sizeof(buff));
  size_t s = st.read(buff, sizeof(buff), ec);
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();
  LOG_TRACE("read buff %s", buff);

  st.write(buff, strlen(buff), ec);
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();

  st.close(ec);
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();

  // client
  st = TcpStream::connect(addr, ec);
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();
  EXPECT_TRUE(st.native_handle() > 0);
  LOG_TRACE("clinet socket value %d", st.native_handle());
  LOG_TRACE("client ip and port %s:%d", addr.get_ip(), addr.get_port());

  memset(buff, 0, sizeof(buff));
  s = st.read(buff, sizeof(buff), ec);
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();
  LOG_TRACE("read buff %s", buff);
  if (s == 0) {
    st.close(ec);
    EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
    ec.clear();
  }

  st.write(buff, strlen(buff), ec);
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();

  st.close(ec);
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();
}
