
#include "utils/random.h"

#include <algorithm>
#include <chrono>
#include <cstring>
#include <random>

namespace {

using time_clock = std::chrono::steady_clock;

struct splitmix64_t {
  constexpr uint64_t operator()(uint64_t &v) const noexcept {
    uint64_t z = (v += 0x9e3779b97f4a7c15ULL);
    z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9ULL;
    z = (z ^ (z >> 27)) * 0x94d049bb133111ebULL;
    return z ^ (z >> 31);
  }
};

inline constexpr splitmix64_t splitmix64{};

struct xorshift32_t {
  constexpr uint32_t operator()(uint32_t &v) const noexcept {
    uint64_t x = v;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    return v = static_cast<uint32_t>(x);
  }
};

inline constexpr xorshift32_t xorshift32;

struct xorshift64_t {
  constexpr uint64_t operator()(uint64_t &v) const noexcept {
    uint64_t x = v;
    x ^= x << 13;
    x ^= x >> 7;
    x ^= x << 17;
    return v = x;
  }
};

inline constexpr xorshift64_t xorshift64;

struct seed_t {
  static constexpr uint64_t kFixSeed = 15011051792192176828u;

  struct fixed_tag_t {};

  constexpr uint64_t operator()(uint64_t &seed) const noexcept {
    return splitmix64(seed);
  }

  template <typename OutputIt>
  constexpr void operator()(fixed_tag_t, OutputIt first,
                            OutputIt last) const noexcept {
    uint64_t x = kFixSeed;
    operator()(first, last, x);
  }

  template <typename OutputIt>
  void operator()(OutputIt first, OutputIt last) const {
    uint64_t x = std::random_device{}() ^
                 (uint64_t(time_clock::now().time_since_epoch().count())) << 32;
    operator()(first, last, x);
  }

  template <typename OutputIt>
  constexpr void operator()(OutputIt first, OutputIt last,
                            uint64_t seed) const noexcept {
    for (; first != last; ++first) {
      *first = splitmix64(seed);
    }
  }
};

inline constexpr seed_t::fixed_tag_t fixed_tag{};
inline constexpr seed_t random_seed{};

class xorshift128 {
  uint32_t seed_[4];

public:
  constexpr xorshift128() noexcept : seed_{} { set_fix_seed(false); }

  constexpr uint32_t operator()() noexcept {
    uint32_t t = seed_[3];

    uint32_t const s = seed_[0]; /* Perform a contrived 32-bit shift. */
    seed_[3] = seed_[2];
    seed_[2] = seed_[1];
    seed_[1] = s;

    t ^= t << 11;
    t ^= t >> 8;
    return seed_[0] = t ^ s ^ (s >> 19);
  }

  constexpr void set_fix_seed(bool fix) noexcept {
    if (fix) {
      random_seed(fixed_tag, seed_, seed_ + 4);
    } else {
      random_seed(seed_, seed_ + 4);
    }
  }
};

class xoshiro256ss {
  uint64_t seed_[4];

  static constexpr uint64_t rol64(uint64_t x, uint64_t k) noexcept {
    return (x << k) | (x >> (64 - k));
  }

public:
  constexpr xoshiro256ss() noexcept : seed_{} { set_fix_seed(false); }

  constexpr uint64_t operator()() noexcept {
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

  constexpr void set_fix_seed(bool fix) noexcept {
    if (fix) {
      random_seed(fixed_tag, seed_, seed_ + 4);
    } else {
      random_seed(seed_, seed_ + 4);
    }
  }

  constexpr float gen_float() noexcept {
    uint64_t s = (*this)();
    s = ((s >> 41) + (0x7fUL << 23));
    float r = {};
    memcpy(&r, &s, sizeof(r));
    return r;
  }

  constexpr double gen_double() noexcept {
    uint64_t s = (*this)();
    s = ((s >> 12) + (0x3ffULL << 52));
    double r = {};
    memcpy(&r, &s, sizeof(r));
    return r;
  }
};

static xoshiro256ss g_xorshfit;

} // namespace

void fix_seed() { g_xorshfit.set_fix_seed(true); }

uint64_t rand_num() { return g_xorshfit(); }

uint64_t rand_scope(uint64_t min_v, uint64_t max_v) {
  uint64_t const diff = std::max(min_v, max_v) - std::min(min_v, max_v);
  if (diff == 0) {
    return max_v;
  }
  return rand_num() % diff + std::min(min_v, max_v);
}
