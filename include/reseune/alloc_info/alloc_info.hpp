#ifndef RESEUNE_ALLOC_INFO_H
#define RESEUNE_ALLOC_INFO_H

namespace reseune {

#define ALLOC_INFO_FIELDS                                                       \
  size_t size;                                                                  \
  char * block_start                                                           

#define PRINT_ALLOC_INFO_FIELDS                                                 \
  print_bits<true, false>("alloc_info is at", uintptr(this));                   \
  print_bits<true, false>("size",       size)

  struct alloc_info {
    ALLOC_INFO_FIELDS;

    void describe_instance() const {
      PRINT_ALLOC_INFO_FIELDS;
    }
  };

  struct alloc_info_with_unfree_flag {
    ALLOC_INFO_FIELDS;

    bool unfree;

    void describe_instance() const {
      PRINT_ALLOC_INFO_FIELDS;
      printf(
        "state               : %s\n",
        unfree
        ? "unfree"
        : "free");
    }
  };
}

#endif
