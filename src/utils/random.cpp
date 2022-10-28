
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

class Seed {
  bool fix_seed_;
  static constexpr uint64_t FIX_SEED = 15011051792192176828u;

public:
  Seed() : fix_seed_(false) {}

  void gen32(uint32_t *dst, size_t size) {
    xorshift32 xor32;
    for (size_t i = 0; i < size; ++i) {
      dst[i] = fix_seed_ ? (FIX_SEED | i) : xor32();
    }
  }

  void gen64(uint64_t *dst, size_t size) {
    xorshift64 xor64;
    for (size_t i = 0; i < size; ++i) {
      dst[i] = fix_seed_ ? (FIX_SEED | i) : xor64();
    }
  }

  void setFix(bool fix) { fix_seed_ = fix; }
};

class xorshift128 {
  uint32_t seed_[4];
  Seed gen_seed_;

public:
  xorshift128() { setFixSeed(false); }

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

  void setFixSeed(bool fix) {
    gen_seed_.setFix(fix);
    gen_seed_.gen32(seed_, 4);
  }
};

class xoshiro256ss {
  uint64_t seed_[4];
  Seed gen_seed_;

  uint64_t rol64(uint64_t x, uint64_t k) { return (x << k) | (x >> (64 - k)); }

public:
  xoshiro256ss() { setFixSeed(false); }

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

  void setFixSeed(bool fix) {
    gen_seed_.setFix(fix);
    gen_seed_.gen64(seed_, 4);
  }

  float genFloat() {
    uint64_t s = (*this)();
    s = ((s >> 41) + (0x7fUL << 23));
    float r = {};
    memcpy(&r, &s, sizeof(r));
    return r;
  }

  double genDouble() {
    uint64_t s = (*this)();
    s = ((s >> 12) + (0x3ffUL << 52));
    double r = {};
    memcpy(&r, &s, sizeof(r));
    return r;
  }
};

xoshiro256ss g_xorshfit;

} // namespace

void fixSeed() { g_xorshfit.setFixSeed(true); }

size_t randNum() { return g_xorshfit(); }

size_t randScope(size_t min_v, size_t max_v) {
  size_t diff = std::max(min_v, max_v) - std::min(min_v, max_v);
  if (diff == 0) {
    return max_v;
  }
  return randNum() % diff + std::min(min_v, max_v);
}
