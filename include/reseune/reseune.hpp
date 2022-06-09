#ifndef RESEUNE_HPP
#define RESEUNE_HPP

#include <type_traits>

namespace reseune {
  static void print_line() {
    size_t line_ix { 0 };

    while (line_ix++ < 131)
      putchar('=');

    putchar('\n');
  }

  template <bool show_int = true, bool show_bits = true, typename T>
  static void print_bits(char const * const descr, T const & v) {    
    static_assert(std::is_unsigned<T>::value, "Does not handle signed Ts yet.");
    static_assert(sizeof(T) <= 8, "Doesn't handle Ts of size > 8 yet.");
    
    printf(
      show_bits
      ? "%-20s: "
      : "%-20s: ",
      descr);

    size_t           ix            { 0  };
    constexpr size_t width         { 72 };
    char             buff[width]   { 0  };
    size_t           buffix        { 0  };

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

      ++ix %= 8;

      if (0 == ix && mask > 1) [[unlikely]]
        PUTCHAR(show_bits
                ? '\''
                : ' ');
    }

#undef PUTCHAR
    
    printf("%s", buff);

    if constexpr(show_int) [[likely]] {      
      printf(show_bits
             ? " ="
             : "  ");

      const char * const tmp = " 0x%%0%ulx = %% 12lu\n";
      char buf[64] {0};

      for (size_t iix = 0; iix < (8 - sizeof(T)) * 2; ++iix)
        putchar(' ');
      
      sprintf(buf, tmp, sizeof(T) << 1);
      
      printf(buf, (uintptr_t)v, (uintptr_t)v);
    }
    else [[unlikely]] {
      putchar('\n');
    }
  }
}

#endif
