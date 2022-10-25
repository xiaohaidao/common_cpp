// Copyright (C) 2022 dobot inc All rights reserved.
// Author: libobo@dobot.cc. Created in 202204

#include "gtest/gtest.h"

#include "utils/random.h"

TEST(Random, random) {
  std::cout << "random num in (899, 1099) : " << rand_scope(899, 1099) << "\n";
  std::cout << "random num in (899, 1099) : " << rand_scope(899, 1099) << "\n";
  std::cout << "random num in (899, 1099) : " << rand_scope(899, 1099) << "\n";
  std::cout << "random num in (899, 1099) : " << rand_scope(899, 1099) << "\n";
  std::cout << "random num in (1099, 899) : " << rand_scope(1099, 899) << "\n";
  std::cout << "random num in (1099, 899) : " << rand_scope(1099, 899) << "\n";
  std::cout << "random num in (1099, 899) : " << rand_scope(1099, 899) << "\n";
  std::cout << "random num in (1099, 899) : " << rand_scope(1099, 899) << "\n";
  std::cout << "random num in (899, 899) : " << rand_scope(899, 899) << "\n";
  std::cout << "random num in (899, 900) : " << rand_scope(899, 900) << "\n";
  std::cout << "random num in (899, 901) : " << rand_scope(899, 901) << "\n";
  std::cout << "random num in (899, 902) : " << rand_scope(899, 902) << "\n";
  std::cout << "random num : " << rand_num() << "\n";
  std::cout << "random num : " << rand_num() << "\n";
  std::cout << "random num : " << rand_num() << "\n";
  std::cout << "random num : " << rand_num() << "\n";
}
