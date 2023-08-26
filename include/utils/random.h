
#ifndef UTILS_RANDOM_H
#define UTILS_RANDOM_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void fixSeed();
uint64_t randNum();
uint64_t randScope(uint64_t min, uint64_t max);

#ifdef __cplusplus
}
#endif

#endif // UTILS_RANDOM_H
