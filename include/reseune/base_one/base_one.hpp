#ifndef RESEUNE_BASE_ONE_HPP
#define RESEUNE_BASE_ONE_HPP

#include "reseune/util/util.hpp"
#include "reseune/alloc_info/alloc_info.hpp"
#include "reseune/doubly_linked/doubly_linked.hpp"

#include <inttypes.h>
#include <stdint.h>

namespace reseune {
  namespace base_one {
    using            alloc_node = doubly_linked<alloc_info>;    
    constexpr        size_t       ALLOC_HEADER_SZ      {offsetof(alloc_node, data.block)};
    constexpr        size_t       MEMORY_WORDS         {1024};
    constexpr        size_t       MEMORY_BYTES         {MEMORY_WORDS << 3};
    static           char *       MEMORY[MEMORY_BYTES] {0};
    static           alloc_node   FREE_LIST {nullptr,nullptr};

#define PRINT(x,y ) print_bits<true,false>((x), (y))
#define LINE print_line()

    inline static void malloc_add_block(void * const addr, size_t size) {
      LINE;
      printf("ADDING NEW MEMORY TO THE FREE LIST!\n");
      LINE;
      PRINT("Given memory at", uintptr(addr));
      PRINT("Given bytes", uintptr(size));
      PRINT("sizeof(alloc_node)", sizeof(alloc_node));
      
      // align the start addr of our block to the next pointer aligned addr
      alloc_node * blk {
        reinterpret_cast<alloc_node *>(align_up(uintptr(addr), sizeof(void *))) };
        
      PRINT("Aligned block to", uintptr(blk));

      // calculate actual size - mgmt overhead
      blk->data.size =
        uintptr(addr)
        + size
        - uintptr(blk)
        - ALLOC_HEADER_SZ;
      
      blk->describe_instance();
      blk->data.describe_instance('-');
      
      PRINT("Add to free list at", uintptr(&FREE_LIST));
      LINE;
      putchar('\n');
      
      blk->insert_after(FREE_LIST);
    }
  }
}

#undef LINE
#undef PRINT 

#endif
