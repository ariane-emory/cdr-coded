#ifndef RESEUNE_BASE_ONE_HPP
#define RESEUNE_BASE_ONE_HPP

namespace reseune {
  namespace base_one {
    constexpr size_t MEMORY_WORDS { 1024 };
    constexpr size_t MEMORY_BYTES { MEMORY_WORDS << 3 };
    
    static char * MEMORY[MEMORY_BYTES] = { 0 };
    
    static doubly_linked<alloc_block> free_list { &free_list, &free_list };

    
    static void malloc_add_block(char * addr, size_t size) {
      alloc_block { addr, size };

      
      //   alloc_block * blk;
      
    //   // align the start addr of our block to the next pointer aligned addr
    //   blk = reinterpret_cast<alloc_block *>(align_up((uintptr_t)addr, sizeof(void*)));

    //   // calculate actual size - mgmt overhead
    //   blk->size = (uintptr_t) addr + size - (uintptr_t) blk 
         //     - ALLOC_HEADER_SZ;

         //   //and now our giant block of MEMORY is added to the list!
         //   blk->node.insert_before(&free_list);
         }
  }
}
#endif
