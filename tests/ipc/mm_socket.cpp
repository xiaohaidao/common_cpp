
#include "gtest/gtest.h"

#include "ipc/mm_socket.h"
#include "utils/log.h"

TEST(IPCTest, MMSocket) {
  const char server[] = "bind_mm";

  mm_socket s;
  int ret = s.bind(server);
  LOG_TRACE("server bind %d", ret);

  mm_client client, s_client;

  // connect
  LOG_TRACE("connect");
  ret = s.accept(s_client);
  LOG_TRACE("server connect %d", ret);
  EXPECT_TRUE(ret > 0);

  ret = client.connect(server);
  LOG_TRACE("client connect %d", ret);
  EXPECT_TRUE(ret > 0);

  ret = s.accept(s_client);
  LOG_TRACE("server connect %d", ret);
  EXPECT_TRUE(ret > 0);

  ret = client.connect(server);
  LOG_TRACE("client connect %d", ret);
  EXPECT_TRUE(ret == 0);

  ret = s.accept(s_client);
  LOG_TRACE("server connect %d", ret);
  EXPECT_TRUE(ret == 0);

  ret = client.connect(server);
  LOG_TRACE("client connect %d", ret);
  EXPECT_TRUE(ret == 0);

  ret = s.accept(s_client);
  LOG_TRACE("server connect %d", ret);
  EXPECT_TRUE(ret > 0);

  ret = client.connect(server);
  LOG_TRACE("client connect %d", ret);
  EXPECT_TRUE(ret == 0);

  bool const ok =
      client.send(nullptr, 0) == 0 && s_client.send(nullptr, 0) == 0;
  LOG_ERROR("ok %d", ok);

  // read and write
  size_t send_size = 0, recv_size = 0;
  size_t i = 0, first_size = 0;
  for (i = 0; i < (size_t)(1024 * 1024 * 8) && ok; ++i) {
    // LOG_TRACE("read write index %d", i);
    char buff[256] = {};
    const char message[] = "client send message";
    char msg_buff[256] = {};
    snprintf(msg_buff, sizeof(msg_buff) - 1, "%s:%.100zu", message, i);
    ret = client.send(msg_buff, strlen(msg_buff) + 1);
    // LOG_TRACE("client send msg size %d:%d", ret, strlen(msg_buff));
    EXPECT_TRUE(ret == strlen(msg_buff) + 1);
    send_size += ret;
    if (!first_size) {
      first_size = ret;
    }
    if (first_size != ret) {
      LOG_DEBUG("diff size %d %d", first_size, ret);
      break;
    }

    ret = s_client.recv(buff, sizeof(buff) - 1);
    // LOG_TRACE("server receive msg size %d:%d", ret, strlen(buff));
    EXPECT_TRUE(ret == strlen(msg_buff) + 1);
    recv_size += ret;
    if (first_size != ret) {
      LOG_DEBUG("diff size %d %d", first_size, ret);
      break;
    }

    ret = s_client.send(buff, strlen(buff) + 1);
    // LOG_TRACE("server send msg size %d:%d", ret, strlen(buff));
    EXPECT_TRUE(ret == strlen(buff) + 1);
    send_size += ret;
    if (first_size != ret) {
      LOG_DEBUG("diff size %d %d", first_size, ret);
      break;
    }

    ret = client.recv(msg_buff, sizeof(msg_buff) - 1);
    // LOG_TRACE("client receive msg size %d:%d", ret, strlen(msg_buff));
    EXPECT_TRUE(ret == strlen(buff) + 1);
    recv_size += ret;
    if (first_size != ret) {
      LOG_DEBUG("diff size %d %d", first_size, ret);
      break;
    }
  }
  LOG_TRACE("count %d send size %d and recv size %d", i, send_size, recv_size);

  // close
  client.close();
  s_client.close();
  s.close();
  LOG_TRACE("mm socket end");
}