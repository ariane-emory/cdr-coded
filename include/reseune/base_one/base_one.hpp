#ifndef RESEUNE_BASE_ONE_HPP
#define RESEUNE_BASE_ONE_HPP

#include "reseune/util/util.hpp"
#include "reseune/alloc_info/alloc_info.hpp"
#include "reseune/doubly_linked/doubly_linked.hpp"

#include <inttypes.h>
#include <stdint.h>
#include <cassert>

namespace reseune {
  namespace base_one {
    // ===========================================================================================================

    using            alloc_node = doubly_linked<alloc_info>;    
    constexpr        size_t       ALLOC_HEADER_SZ      {offsetof(alloc_node, data.block)};
    constexpr        size_t       MIN_ALLOC_SZ         {ALLOC_HEADER_SZ + 32};
    constexpr        size_t       MEMORY_WORDS         {1024};
    constexpr        size_t       MEMORY_BYTES         {MEMORY_WORDS << 3};
    static           char *       MEMORY[MEMORY_BYTES] {0};
    static           alloc_node   FREE_LIST {nullptr,nullptr};

    // ===========================================================================================================

#define PRINT(x,y) print_bits<true,false>((x), (y))
#define LINE print_line()

    // ===========================================================================================================

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
      
      // blk->describe_instance();
      blk->data.describe_instance('-');
      
      PRINT("Add to free list at", uintptr(&FREE_LIST));
      LINE;
      putchar('\n');
      
      blk->insert_after(FREE_LIST);
    }

    // ===========================================================================================================

    void * alloc(size_t size)
    {
      void       * ptr {nullptr};
      alloc_node * blk {nullptr};

      assert(size > 0);
      
      // Align the pointer
      size = align_up(size, sizeof(void *));

      // try to find a big enough block to alloc
      for (auto & b : FREE_LIST)
      {
        if (b.size >= size)
        {
          ptr = &b.block;
          break;
        }
      }

      // // we found something
      // if(ptr)
      // {
      //   // Can we split the block?
      //   if((blk->size - size) >= MIN_ALLOC_SZ)
      //   {
      //     alloc_node_t* new_blk;
      //     new_blk = (alloc_node_t*)((uintptr_t)(&blk->block) + size);
      //     new_blk->size = blk->size - size - ALLOC_HEADER_SZ;
      //     blk->size = size;
      //     list_add_(&new_blk->node, &blk->node, blk->node.next);
      //   }

      //   list_del(&blk->node);
      // }

      // } // else NULL

      return ptr;
    }
    
#undef PRINT 
#undef LINE
  }
}

#endif
