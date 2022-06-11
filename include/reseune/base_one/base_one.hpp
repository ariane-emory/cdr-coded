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
    constexpr        size_t       ALLOC_HEADER_SZ      {offsetof(alloc_node, data.block_start_pointer)};
    constexpr        size_t       MIN_ALLOC_SZ         {ALLOC_HEADER_SZ + 32};
    constexpr        size_t       MEMORY_WORDS         {1024};
    constexpr        size_t       MEMORY_BYTES         {MEMORY_WORDS << 3};
    static           char         MEMORY[MEMORY_BYTES] {0};
    static           alloc_node   FREE_LIST {nullptr,nullptr};

    // ===========================================================================================================

#define PRINT(x,y) print_bits<true,false>((x), (y))
#define LINE print_line()
#define HLINE print_line('-')
#define PROFFSET(x) PRINT("With offset", uintptr(&x) - uintptr(MEMORY))
    // ===========================================================================================================

    inline static void alloc_add_block(void * const addr, size_t size) {
      LINE;
      printf("ADDING NEW MEMORY TO THE FREE LIST @ %zu!\n", MEMORY);
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

    void initialize() {
      alloc_add_block(MEMORY, MEMORY_BYTES);
    }

    // ===========================================================================================================
    
    void describe_free_list() {
      assert(nullptr != FREE_LIST.next);
      
      LINE;
      printf("PRINTING THE FREE LIST @ %zu!\n", MEMORY);
      LINE;

      size_t ix {0};
      for (const auto & x : *FREE_LIST.next) {
        printf("Node                : #%u\n", ++ix);
        PRINT("Node is at", uintptr(&x));
        PROFFSET(x);
        HLINE;
        x.describe_instance();
        x.data.describe_instance('-');
        LINE;
      }
  
      putchar('\n');
    }

    // ===========================================================================================================
    
    void * alloc(size_t size)
    {
      LINE;
      printf("ALLOCATING MEMORY FROM THE FREE LIST @ %zu!\n", MEMORY);
      LINE;
      PRINT("Bytes requested: ", size);
      
      void *       pointer {nullptr};
      alloc_node * blk {nullptr};

      assert(size > 0);
      
      // Align the pointer
      size = align_up(size, sizeof(void *));

      // try to find a big enough block to alloc
      for (auto & b : FREE_LIST)
        if (b.data.size >= size)
        {
          blk = &b;
          pointer = &b.data.block_start_pointer;

          PRINT("Selected block at", uintptr(pointer));
          PRINT("With offset", uintptr(pointer) - uintptr(MEMORY));
          HLINE;
          blk->describe_instance();
          blk->data.describe_instance('-');
          LINE;
          
          break;
        }

      // PRINT("Selected block at", uintptr(&blk->data.block_start_pointer));
      
      if (nullptr == blk)
        return pointer;

      // Can we split the block?
      if ((blk->data.size - size) >= MIN_ALLOC_SZ)
      {
        alloc_node * new_blk;
        new_blk = reinterpret_cast<alloc_node *>((uintptr(pointer) + size));
        
        new_blk->data.size = blk->data.size - size - ALLOC_HEADER_SZ;
        blk->data.size = size;

        new_blk->insert_before(blk);

        PRINT("Created new block at", uintptr(&new_blk));
        PRINT("With offset", uintptr(&new_blk) - uintptr(MEMORY));
        HLINE;
        new_blk->describe_instance();
        new_blk->data.describe_instance();
        HLINE;
        //   list_add_(&new_blk->node, &blk->node, blk->node.next);
      }

      blk->remove();
      
      // list_del(&blk->node);

      PRINT("Gave pointer to", uintptr(pointer));
      LINE;
      
      return pointer;
    }
    
#undef PRINT 
#undef LINE
  }
}

#endif
