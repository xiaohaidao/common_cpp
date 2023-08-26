
#ifndef MAX_HEAP_H
#define MAX_HEAP_H

#include "min_heap.h"

#if __cplusplus >= 201103L
template <typename T> using max_heap = min_heap<T, std::greater<T> >;
#else
template <typename T> class max_heap : public min_heap<T, std::greater<T> > {};
#endif

#endif // MAX_HEAP_H
