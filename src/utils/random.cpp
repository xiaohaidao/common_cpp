
#include "utils/random.h"

#include <algorithm>
#include <chrono>
#include <cstring>

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

class xoshiro256ss {
  uint64_t seed_[4];
  uint64_t rol64(uint64_t x, uint64_t k) { return (x << k) | (x >> (64 - k)); }

public:
  xoshiro256ss() {
    xorshift64 xor64;
    seed_[0] = xor64();
    seed_[1] = xor64();
    seed_[2] = xor64();
    seed_[3] = xor64();
  }

  uint64_t operator()() {
    uint64_t *s = seed_;
    uint64_t const result = rol64(s[1] * 5, 7) * 9;
    uint64_t const t = s[1] << 17;
    s[2] ^= s[0];
    s[3] ^= s[1];
    s[1] ^= s[2];
    s[0] ^= s[3];

    s[2] ^= t;
    s[3] = rol64(s[3], 45);
    return result;
  }

  float gen_float() {
    uint64_t s = (*this)();
    s = ((s >> 41) + (0x7fUL << 23));
    float r = {};
    memcpy(&r, &s, sizeof(r));
    return r;
  }

  double gen_double() {
    uint64_t s = (*this)();
    s = ((s >> 12) + (0x3ffUL << 52));
    double r = {};
    memcpy(&r, &s, sizeof(r));
    return r;
  }

};

xoshiro256ss g_xorshfit;

} // namespace

size_t rand_num() { return g_xorshfit(); }

size_t rand_scope(size_t min, size_t max) {
  if (max == min)
    return max;
  return rand_num() % (std::max(min, max) - std::min(min, max)) +
         std::min(min, max);
}
