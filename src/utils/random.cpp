
#include "utils/random.h"

#include <algorithm>
#include <chrono>

namespace {

typedef std::chrono::time_point<std::chrono::steady_clock> time_type;

class xorshift32 {
  uint32_t seed_;

public:
  xorshift32() : seed_(time_type::clock::now().time_since_epoch().count()) {}

  uint32_t operator()() {
    uint64_t x = seed_;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    return seed_ = x;
  }
};

class xorshift64 {
  uint64_t seed_;

public:
  xorshift64() : seed_(time_type::clock::now().time_since_epoch().count()) {}

  uint64_t operator()() {
    uint64_t x = seed_;
    x ^= x << 13;
    x ^= x >> 7;
    x ^= x << 17;
    return seed_ = x;
  }
};

class xorshift128 {
  uint32_t seed_[4];

public:
  xorshift128() {
    xorshift32 xor32;
    seed_[0] = xor32();
    seed_[1] = xor32();
    seed_[2] = xor32();
    seed_[3] = xor32();
  }

  uint32_t operator()() {
    uint32_t t = seed_[3];

    uint32_t s = seed_[0]; /* Perform a contrived 32-bit shift. */
    seed_[3] = seed_[2];
    seed_[2] = seed_[1];
    seed_[1] = s;

    t ^= t << 11;
    t ^= t >> 8;
    return seed_[0] = t ^ s ^ (s >> 19);
  }
};

xorshift64 g_xorshfit;

} // namespace

size_t rand_num() { return g_xorshfit(); }

size_t rand_scope(size_t min, size_t max) {
  return rand_num() % (std::max(min, max) - std::min(min, max)) +
         std::min(min, max);
}
