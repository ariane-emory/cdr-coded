#ifndef RESEUNE_ALLOC_INFO_H
#define RESEUNE_ALLOC_INFO_H

namespace reseune {
  class alloc_info {
  public:
    size_t      size;
    char *      block;

    void describe_instance(char c = '=') const {
      print_line(c);
      print_bits<true, false>("alloc_info is at", reinterpret_cast<uintptr_t>(this));
      print_bits<true, false>("size",       size);
      print_bits<true, false>("block",       reinterpret_cast<uintptr_t>(block));
    }
  };
}

#endif
