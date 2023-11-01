
#ifndef UTILS_RANDOM_H
#define UTILS_RANDOM_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void fix_seed();
uint64_t rand_num();
uint64_t rand_scope(uint64_t min, uint64_t max);

#ifdef __cplusplus
}
#endif

#endif // UTILS_RANDOM_H
