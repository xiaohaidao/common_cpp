
#ifndef UTILS_RANDOM_H
#define UTILS_RANDOM_H

#include <inttypes.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

size_t rand_num();
size_t rand_scope(size_t min, size_t max);

#ifdef __cplusplus
}
#endif

#endif // UTILS_RANDOM_H
