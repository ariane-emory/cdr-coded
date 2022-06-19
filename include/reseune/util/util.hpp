#ifndef RESEUNE_UTIL_HPP
#define RESEUNE_UTIL_HPP

#include <cstddef>
#include <cstdio>
#include <inttypes.h>
#include <stdint.h>
#include <type_traits>
#include <chrono>

namespace reseune {
  template <bool(*fun)(const char)>
  
  bool invert_char_predicate(const char c) {
    return ! fun(c);
  }
  
  void measure_time(void(*fun)()) {
#define NOW (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()))    
    auto before = NOW;
    fun();
    auto after  = NOW;
#undef NOW
    
    printf("Took %u ms.\n", after - before);
  }

  template <char c>
  inline bool is_char(const char other_c) {
    return c == other_c;

  }
  
  inline bool is_whitespace(const char c) {
    // printf("Examine '%c' (%u).\n", c, c);
    return ((c == ' ')
            || (c == 9)
            || (c == 10)
            || (c == 11)
            || (c == 12)
            || (c == 13));
  }

  // C-style type
  inline int iswhitespace(int c) {
    return ((c == ' ')
            || (c == 9)
            || (c == 10)
            || (c == 11)
            || (c == 12)
            || (c == 13));
  }

  template <typename T>
  static uintptr_t uintptr(T t) {
    return reinterpret_cast<uintptr_t>(t);
  }

  static constexpr size_t align_up(size_t num, size_t align) {
    return ((num) + ((align)-1)) & ~((align)-1);
  }

  static void print_line(char c = '=') {
    size_t line_ix { 0 };

    while (line_ix++ < 137)
      putchar(c);

    putchar('\n');
  }

  template <bool show_int = true, bool show_bits = true, typename T>
  static void print_bits(const T & v) {
    print_bits<show_int, show_bits>(static_cast<const char *>("(no desc)"), v);
  }
  
  template <bool show_int = true, bool show_bits = true, typename T>
  static void print_bits(const char * descr, const T & v) {    

    static_assert(std::is_integral<T>::value, "Does not handle non-integral Ts yet.");
    static_assert(std::is_unsigned<T>::value, "Does not handle signed Ts yet.");
    static_assert(sizeof(T) <= 8, "Doesn't handle Ts of size > 8 yet.");
    
    printf(
      show_bits
      ? "%-20s: "
      : "%-20s: ",
      descr);

    constexpr size_t  width       { 72 };
    char             buff[width] { 0  };
    size_t           buffix      { 0  };
    size_t           sepix       { 0  };

#define PUTCHAR(x) buff[buffix++] = x

    for (size_t iix = 0; iix < (8 - sizeof(T)) * 9; ++iix)
      PUTCHAR(' ');
    

    for (T mask { static_cast<T>(1) << ((sizeof(T) << 3) - 1) };
         mask;
         mask >>= 1) {
      if constexpr (show_bits)
        PUTCHAR((mask & v)
                ? '1'
                : '0');
      else
        PUTCHAR(' ');

      ++sepix %= 8;

      if (0 == sepix && mask > 1) [[unlikely]]
        PUTCHAR(show_bits
                ? '\''
                : ' ');
    }

#undef PUTCHAR
    
    printf("%s", buff);

    if constexpr(show_int) {
      printf(show_bits
             ? " ="
             : "  ");

      constexpr const char * tmp = " 0x%%0%ulx = %% 19lu\n";
      char buf[64] {0};

      for (size_t iix = 0; iix < (8 - sizeof(T)) * 2; ++iix)
        putchar(' ');
      
      sprintf(buf, tmp, 1 + (sizeof(T) << 1));
      
      printf(buf, (uintptr_t)v, (uintptr_t)v);
    }
    else { 
      putchar('\n');
    }
  }
}

#endif
