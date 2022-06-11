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
    constexpr        size_t       ALLOC_HEADER_SZ      {offsetof(alloc_node, data.block_start)};
    constexpr        size_t       MIN_ALLOC_SZ         {ALLOC_HEADER_SZ + 32};
    constexpr        size_t       MEMORY_WORDS         {1024};
    constexpr        size_t       MEMORY_BYTES         {MEMORY_WORDS << 3};
    static           char         MEMORY[MEMORY_BYTES] {0};
    static           alloc_node   FREE_LIST {nullptr,nullptr};

    // ===========================================================================================================

#define PRINT(x,y) print_bits<true,false>((x), uintptr(y))
#define PROFFSET(x) PRINT("... with offset", uintptr(x) - uintptr(MEMORY))
#define PRINTWOFFSET(x,y) PRINT(x, y); PROFFSET(x)
#define LINE print_line()
#define HLINE print_line('-')

    // ===========================================================================================================

    inline static void alloc_add_block(void * const addr, size_t size) {
      LINE;
      printf("ADDING NEW MEMORY TO THE FREE LIST @ 0x%lx!\n", &FREE_LIST);
      LINE;
      PRINT("Given memory at", addr);
      PRINT("Given bytes", size);
      // PRINT("sizeof(alloc_node)", sizeof(alloc_node));
      
      // align the start addr of our block to the next pointer aligned addr
      alloc_node * blk {
        reinterpret_cast<alloc_node *>(align_up(uintptr(addr), sizeof(void *))) };
        
      PRINT("Aligned block to", blk);

      // calculate actual size - mgmt overhead
      blk->data.size =
        uintptr(addr)
        + size
        - uintptr(blk)
        - ALLOC_HEADER_SZ;
      
      // blk->describe_instance();
      HLINE;
      blk->insert_after(FREE_LIST);
      blk->describe_instance('-');
      blk->data.describe_instance('-');

      LINE;
      putchar('\n');
      
    }

    // ===========================================================================================================

    void initialize() {
      alloc_add_block(MEMORY, MEMORY_BYTES);
    }

    // ===========================================================================================================
    
    void describe_free_list() {
      assert(nullptr != FREE_LIST.next);
      
      LINE;
      printf("PRINTING THE FREE LIST @ 0x%lx!\n", &FREE_LIST);
      LINE;

      size_t ix {0};
      for (const auto & x : *FREE_LIST.next) {
        printf("Node                : #%u\n", ++ix);
        PRINT("Node is at", &x);
        PROFFSET(&x);
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
      printf("ALLOCATING MEMORY FROM THE FREE LIST @ 0x%lx!\n", &FREE_LIST);
      LINE;
      PRINT("Bytes requested: ", size);
      
      void *       pointer {nullptr};
      alloc_node * blk     {nullptr};

      assert(size > 0);
      
      // Align the pointer
      size = align_up(size, sizeof(void *));

      // try to find a big enough block to alloc
      for (auto & b : FREE_LIST)
        if (b.data.size >= size)
        {
          blk = &b;
          pointer = &b.data.block_start;

          PRINT("Selected block at", &b);
          PROFFSET(&b);
          PRINT("With block start at", &b.data.block_start);
          PROFFSET(&b.data.block_start);
          HLINE;
          
          blk->describe_instance();
          blk->data.describe_instance('-');
          LINE;
          
          break;
        }

      // PRINT("Selected block at", uintptr(&blk->data.block_start));
      
      if (nullptr == blk)
        return pointer;

      // Can we split the blocko?
      if ((blk->data.size - size) >= MIN_ALLOC_SZ)
      {
        alloc_node * new_blk;
        new_blk = reinterpret_cast<alloc_node *>((uintptr(pointer) + size));
        
        new_blk->data.size = blk->data.size - size - ALLOC_HEADER_SZ;
        blk->data.size = size;

        new_blk->insert_before(blk);

        blk->remove();
        
        PRINT("Created new block at", new_blk);
        PROFFSET(new_blk);
        PRINT("With block start at", &new_blk->data.block_start);
        PROFFSET(&new_blk->data.block_start);

        HLINE;
        new_blk->describe_instance();
        new_blk->data.describe_instance();
        HLINE;
        //   list_add_(&new_blk->node, &blk->node, blk->node.next);
      }

      
      // list_del(&blk->node);

      PRINT("Gave pointer to", uintptr(pointer));
      LINE;
      
      return pointer;
    }

    // ===========================================================================================================

    void release(void * pointer) {

      assert(nullptr != pointer);

      LINE;
      printf("RELEASING 0x%lx!\n", uintptr(pointer));
      LINE;

      alloc_node * blk;
      // alloc_node * free_blk;

      blk = reinterpret_cast<alloc_node *>(uintptr(pointer) - ALLOC_HEADER_SZ);
      PRINT("It's node is", blk);
      
      // Let's put it back in the proper spot
      // list_for_each_entry(free_blk, &free_list, node)
      // {
      //   if(free_blk > blk)
      //   {
      //     list_add_(&blk->node, free_blk->node.prev, &free_blk->node);
      //     goto blockadded;
      //   }
      // }
      // list_add_tail(&blk->node, &free_list);
 
      for (auto & free_blk : *FREE_LIST.next) {
        if (&free_blk > blk) {
          blk->insert_before(free_blk);
        }
      }
      
      // blockadded:
      //     // Let's see if we can combine any memory
      //     defrag_free_list();
    }
  }
}

// ===========================================================================================================
    
#undef PRINT
#undef PROFFSET
#undef PRINTWOFFSET
#undef LINE
#undef HLINE

#endif
