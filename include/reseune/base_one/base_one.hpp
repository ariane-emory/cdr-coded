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
    extern           char *       MEMORY[MEMORY_BYTES];
    extern           alloc_node   FREE_LIST;

#define PRINT(x,y ) print_bits<true,false>((x), (y))
    
    inline static void malloc_add_block(void * const addr, size_t size) {
      print_line();
      PRINT("Addr is at", uintptr(addr));
      PRINT("Block size", sizeof(alloc_node));
      
      // align the start addr of our block to the next pointer aligned addr
      alloc_node * blk {
        reinterpret_cast<alloc_node *>(
          align_up(uintptr(addr), sizeof(void *))) };
        
      PRINT("Block is at", uintptr(blk));

      // calculate actual size - mgmt overhead
      blk->data.size =
        uintptr(addr)
        + size
        - uintptr(blk)
        - ALLOC_HEADER_SZ;
      
      blk->describe_instance();
      blk->data.describe_instance('-');
      
      //   //and now our giant block of MEMORY is added to the list!
      //   blk->node.insert_before(&free_list);

      PRINT("Attach to free list at", uintptr(&FREE_LIST));

      blk->insert_after(FREE_LIST);
    }
  }
}

#undef PRINT 

#endif
