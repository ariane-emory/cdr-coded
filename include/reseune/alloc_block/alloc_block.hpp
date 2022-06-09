#ifndef RESEUNE_ALLOC_BLOCK_H
#define RESEUNE_ALLOC_BLOCK_H

#define ALLOC_HEADER_SZ offsetof(alloc_block, block)

#ifndef align_up
#define align_up(num, align) (((num) + ((align)-1)) & ~((align)-1))
#endif

namespace reseune {
  class alloc_block {
  public:
    double_link node;
    size_t      size;
    char *      block;

    static double_link free_list;
    
    constexpr alloc_block(): node({}), size(0), block(nullptr) {}

    static void malloc_add_block(void * addr, size_t size) {
      alloc_block * blk;
      
      // align the start addr of our block to the next pointer aligned addr
      blk = reinterpret_cast<alloc_block *>(align_up((uintptr_t)addr, sizeof(void*)));

      // calculate actual size - mgmt overhead
      blk->size = (uintptr_t) addr + size - (uintptr_t) blk 
     - ALLOC_HEADER_SZ;

      //and now our giant block of memory is added to the list!
      blk->node.insert_before(&free_list);
    }
  };
}

#endif
