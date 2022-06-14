#ifndef RESEUNE_ALLOC_INFO_H
#define RESEUNE_ALLOC_INFO_H

namespace reseune {

  // ===================================================================================================================

  struct alloc_info_with_unfree_flag {
    size_t size;
    char * block_start;
    bool   unfree;


    void describe_instance() const {
      print_bits<true, false>("alloc_info is at", uintptr(this));
      print_bits<true, false>("size", size);
      printf(
        "state               : %s\n",
        unfree
        ? "unfree"
        : "free");
    }
  };

  // ===================================================================================================================

  template <>
  inline void allocator<alloc_info_with_unfree_flag>::commit_block(
    allocator<alloc_info_with_unfree_flag>::alloc_node & block) {
    block.data.unfree = true;
  }

  template <>
  inline void allocator<alloc_info_with_unfree_flag>::release_block(
    allocator<alloc_info_with_unfree_flag>::alloc_node & pblock, bool verbose) {
    pblock.data.unfree = false;
  }
  
  template <>
  inline bool allocator<alloc_info_with_unfree_flag>::is_free(
    allocator<alloc_info_with_unfree_flag>::alloc_node & block) {    
    return ! block.data.unfree;
  }    
}

#endif
