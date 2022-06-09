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
      ? "%s: 0b"
      : "%s:   ",
      descr);

    size_t ix { 0 };
            
    for (T mask { static_cast<T>(1) << ((sizeof(T) << 3) - 1) };
         mask;
         mask >>= 1) {
      if (show_bits)
        putchar((mask & v) ? '1' : '0');
      else
        putchar(' ');

      ++ix %= 8;

      if (0 == ix && mask > 1) [[unlikely]]
        putchar(show_bits
                ? '\''
                : ' ');
    }
            
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
