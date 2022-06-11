#ifndef RESEUNE_BASE_ONE_HPP
#define RESEUNE_BASE_ONE_HPP

namespace reseune {
  namespace base_one {
    using     alloc_block = doubly_linked<alloc_info>;

    constexpr size_t        MEMORY_WORDS         { 1024 };
    constexpr size_t        MEMORY_BYTES         { MEMORY_WORDS << 3 };

    static    char *        MEMORY[MEMORY_BYTES] { 0 };
    static    alloc_block   FREE_LIST            { &FREE_LIST, &FREE_LIST };
    
    static void malloc_add_block(void * addr, size_t size) {
      print_line();
      print_bits<true,false>("Block size", sizeof(alloc_block));
      
      alloc_block * blk;
      blk = reinterpret_cast<alloc_block *>(align_up(reinterpret_cast<uintptr_t>(addr), sizeof(void *)));
        
      print_bits<true,false>("Block is at", reinterpret_cast<uintptr_t>(blk));
      
      //   // align the start addr of our block to the next pointer aligned addr
        //   blk = reinterpret_cast<alloc_info *>(align_up((uintptr_t)addr, sizeof(void*)));

      //   // calculate actual size - mgmt overhead
          //   blk->size = (uintptr_t) addr + size - (uintptr_t) blk 
                 //     - ALLOC_HEADER_SZ;

        //   //and now our giant block of MEMORY is added to the list!
        //   blk->node.insert_before(&free_list);
        }
  }
}
#endif
