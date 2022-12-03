
#ifndef UTILS_UTF_H
#define UTILS_UTF_H

#include <codecvt>
#include <locale>

namespace utf {

inline std::u16string convert(const char *first, const char *last) {
  return std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t>()
      .from_bytes(first, last);
}

inline std::u16string convert(const char *src) {
  return std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t>()
      .from_bytes(src);
}

inline std::string convert(const char16_t *first, const char16_t *last) {
  return std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t>()
      .to_bytes(first, last);
}

inline std::string convert(const char16_t *src) {
  return std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t>()
      .to_bytes(src);
}

} // namespace utf

#endif // UTILS_UTF_H
