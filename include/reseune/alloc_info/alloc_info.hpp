#ifndef RESEUNE_ALLOC_INFO__H
#define RESEUNE_ALLOC_INFO__H

namespace reseune {
  struct alloc_info {
    size_t size;
    char * block_start;

    void describe_instance() const {
      print_bits<true, false>("alloc_info is at", uintptr(this));
      print_bits<true, false>("size", size);
    }
  };
}

#endif
