
#ifndef UTILS_MACRO_H
#define UTILS_MACRO_H

#define CHECK_TRUE(check, re)                                                  \
  do {                                                                         \
    if (check) {                                                               \
      return re;                                                               \
    }                                                                          \
  } while (false)

#define CHECK_FALSE(check, re) CHECK_TRUE(!check, re)

#define CHECK_EC CHECK_TRUE

#endif // UTILS_MACRO_H
