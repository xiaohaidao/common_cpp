
#ifndef UTILS_RANDOM_H
#define UTILS_RANDOM_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

void fixSeed();
size_t randNum();
size_t randScope(size_t min, size_t max);

#ifdef __cplusplus
}
#endif

#endif // UTILS_RANDOM_H
