#ifndef _BASE36_H
#define _BASE36_H
#include <cstdint>
#include <stdexcept>
namespace pitchstream {
std::uint64_t base36_max = 4738381338321616895LL;

  /***
   * Convert a range of characters into base36 number. Accepts 0-9 and A-Z as digits
   * type is a forward iterator to a char
   * @param begin iterator to a begining of number to convert
   * @param end iterator to an ending of number to convert
   */
  template<typename T>
  std::uint64_t base36(T begin, const T & end) {
    if (end - begin > 12) {
	throw std::invalid_argument("Input string too long (12 digits at most)");      
    }
    std::uint64_t result = 0;
    while (begin != end) {
      int d;
      if (*begin >= '0' && *begin<='9')
	d = *begin - '0';
      else if (*begin >= 'A' && *begin<='Z')
	d = *begin - 'A' + 10;
      else
	throw std::invalid_argument("Not a base36 digit");
      result *= 36;
      result += d;

      begin++;
    }

    return result;
  };

  /***
   * Convert a string-equivalent to base36 number
   * @param s string-equivalent input, at most 12-characters long
   */
  template<typename T>
  std::uint64_t base36(T s) {
    return base36(s.begin(), s.end());
  }

}
#endif
