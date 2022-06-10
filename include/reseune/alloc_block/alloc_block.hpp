#ifndef RESEUNE_ALLOC_BLOCK_H
#define RESEUNE_ALLOC_BLOCK_H

#define ALLOC_HEADER_SZ offsetof(alloc_block, block)

namespace reseune {
  class alloc_block {
  public:
    size_t      size;
    char *      block;

    constexpr alloc_block(): size(0), block(nullptr) {}
  };

  static doubly_linked<alloc_block> free_list;

  // static void malloc_add_block(void * addr, size_t size) {
  //   alloc_block * blk;
      
  //   // align the start addr of our block to the next pointer aligned addr
  //   blk = reinterpret_cast<alloc_block *>(align_up((uintptr_t)addr, sizeof(void*)));

  //   // calculate actual size - mgmt overhead
  //   blk->size = (uintptr_t) addr + size - (uintptr_t) blk 
       //     - ALLOC_HEADER_SZ;

       //   //and now our giant block of memory is added to the list!
       //   blk->node.insert_before(&free_list);
       // }
}

#endif
