#ifndef RESEUNE_HPP
#define RESEUNE_HPP

namespace reseune {
  template <typename T>
  static
  void print_bits(
    char const * descr,
    T const & v,
    bool const & show_int = true,
    bool const & show_bits = true) {

    printf(
      show_bits
      ? "%-20s: 0b"
      : "%-20s:   ",
      descr);

    size_t           ix            { 0 };
    constexpr size_t width         { 75 };
    char             buff[width]   { 0 };
    size_t           buffix        { 0 };

    
    //#define PUTCHAR(x) putchar(x)
#define PUTCHAR(x) buff[buffix++] = x
    
    for (T mask { static_cast<T>(1) << ((sizeof(T) << 3) - 1) };
         mask;
         mask >>= 1) {
      if (show_bits)
        PUTCHAR((mask & v) ? '1' : '0');
      else
        PUTCHAR(' ');

      ++ix %= 8;

      if (0 == ix && mask > 1) [[unlikely]]
        PUTCHAR(show_bits
                ? '\''
                : ' ');
    }




    while (buffix < width - 1) PUTCHAR(' ');
    
    printf("%s", buff);

    if (show_int) [[likely]]
      printf(show_bits
             ? " = 0x%016lx = % 20lu\n"
             : "   0x%016lx = % 20lu\n",
             (uintptr_t)v,
             (uintptr_t)v);
    else [[unlikely]]
      putchar('\n');
  }
}

#endif
