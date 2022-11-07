// Copyright (C) 2022 dobot inc All rights reserved.
// Author: libobo@dobot.cc. Created in 202204

#include "gtest/gtest.h"

#include "ipc/MsgQueue.h"
#include "utils/log.h"

TEST(IPCTest, MsgQueueTest) {
#ifndef WIN32
  std::error_code ec;
  ipc::MsgQueue::create(3, ec);
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();

  ipc::MsgQueue::create(3, ec);
  EXPECT_TRUE(ec) << ec.value() << " : " << ec.message();
  ec.clear();

  ipc::MsgQueue::create("/msg_queue", ec);
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();

  auto msg1 = ipc::MsgQueue::open(3, ec);
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();

  auto msg2 = ipc::MsgQueue::open("msg_queue", ec);
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();

  for (size_t i = 0; i < 5; ++i) {
    char buff[] = "gdsgkjadogijaoiejgoaeri g";
    msg1.send(buff, sizeof(buff), ec);
    EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
    ec.clear();

    char recbuff[8192] = {};
    msg1.recv(recbuff, sizeof(recbuff), ec);
    EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
    EXPECT_TRUE(memcmp(recbuff, buff, sizeof(buff)) == 0) << recbuff;
    ec.clear();

    msg1.recvTimeout(recbuff, sizeof(recbuff), 2, ec);
    EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
    ec.clear();
  }

  msg1.remove(ec);
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();
  msg1.remove(ec);
  EXPECT_TRUE(ec) << ec.value() << " : " << ec.message();
  ec.clear();
  msg2.remove(ec);
  EXPECT_FALSE(ec) << ec.value() << " : " << ec.message();
  ec.clear();
#endif // WIN32
}
