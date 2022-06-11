#ifndef RESEUNE_ALLOC_INFO_H
#define RESEUNE_ALLOC_INFO_H

namespace reseune {
  struct alloc_info {
    size_t      size;
    char *      block;

    void describe_instance(char c = '=') const {
      print_line(c);
      print_bits<true, false>("alloc_info is at", uintptr(this));
      print_bits<true, false>("size",       size);
      print_bits<true, false>("block",       uintptr(block));
    }
  };
}

#endif
