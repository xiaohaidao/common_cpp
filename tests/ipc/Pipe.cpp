
#include "gtest/gtest.h"

#include "ipc/Pipe.h"
#include "utils/log.h"

TEST(IPCTest, AnonymousPipeTest) {
  using ipc::Pipe;
  std::error_code ec;
  Pipe pipe = Pipe::create(ec);
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();

  char buff[] = "test pipe buff message!";
  char recv_buff[sizeof(buff)] = {};

  pipe.write(buff, strlen(buff), ec);
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();

  pipe.read(recv_buff, sizeof(recv_buff), ec);
  LOG_INFO("recv message %s", recv_buff);
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();
  memset(recv_buff, 0, sizeof(recv_buff));

  pipe.close(ec);
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();
}

/*
TEST(IPCTest, PipeTest) {
  using ipc::Pipe;
  std::error_code ec;
  Pipe pipe = Pipe::create("test_pipe_name", ec);
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  if (ec) {
    ec.clear();
    pipe = Pipe::connect("test_pipe_name", ec);
    EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  }
  ec.clear();

  Pipe client_pipe = Pipe::connect("test_pipe_name", ec);
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();

  char buff[] = "test pipe buff message!";
  char recv_buff[sizeof(buff)] = {};

  pipe.write(buff, strlen(buff), ec);
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();

  client_pipe.read(recv_buff, sizeof(recv_buff), ec);
  LOG_INFO("recv message %s", recv_buff);
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();
  memset(recv_buff, 0, sizeof(recv_buff));

  client_pipe.write(buff, strlen(buff), ec);
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();

  pipe.read(recv_buff, sizeof(recv_buff), ec);
  LOG_INFO("recv message %s", recv_buff);
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();
  memset(recv_buff, 0, sizeof(recv_buff));

  pipe.close(ec);
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();

  // // The following does not support on windows
  // client_pipe.write(buff, strlen(buff), ec);
  // EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  // ec.clear();
  // client_pipe.read(recv_buff, sizeof(recv_buff), ec);
  // LOG_INFO("recv message %s", recv_buff);
  // EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  // ec.clear();
  // memset(recv_buff, 0, sizeof(recv_buff));

  client_pipe.close(ec);
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();
}

*/
