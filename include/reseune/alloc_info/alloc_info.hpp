#ifndef RESEUNE_ALLOC_INFO_WITH_UNFREE_FLAG_H
#define RESEUNE_ALLOC_INFO_WITH_UNFREE_FLAG_H

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
