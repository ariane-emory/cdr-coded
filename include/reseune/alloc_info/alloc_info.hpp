#ifndef RESEUNE_ALLOC_INFO_H
#define RESEUNE_ALLOC_INFO_H

#define ALLOC_HEADER_SZ offsetof(alloc_info, block)

namespace reseune {
  class alloc_info {
  public:
    size_t      size;
    char *      block;

    void describe_instance() const {
      print_line('-');
      print_bits<true, false>("alloc_info is at", reinterpret_cast<uintptr_t>(this));
      print_bits<true, false>("size",       size);
      print_bits<true, false>("block",       reinterpret_cast<uintptr_t>(block));
    }
  };
}

#endif
