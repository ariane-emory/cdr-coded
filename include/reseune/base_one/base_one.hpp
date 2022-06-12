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
#define FREE_LIST_HEAD (*FREE_LIST.next)

    // ===========================================================================================================

    inline static void alloc_add_block(void * const addr, size_t size) {
      LINE;
      printf("ADDING NEW MEMORY TO THE FREE LIST @ 0x%lx = %ul!\n", &FREE_LIST, &FREE_LIST);
      LINE;
      PRINT("Given memory at", addr);
      PRINT("Given bytes", size);
      // PRINT("sizeof(alloc_node)", sizeof(alloc_node));
      
      // align the start addr of our block to the next pointer aligned addr
      alloc_node * block {
        reinterpret_cast<alloc_node *>(align_up(uintptr(addr), sizeof(void *))) };
        
      PRINT("Aligned block to", block);

      // calculate actual size - mgmt overhead
      block->data.size =
        uintptr(addr)
        + size
        - uintptr(block)
        - ALLOC_HEADER_SZ;
      
      // block->describe_instance();
      // HLINE;
      block->insert_after(FREE_LIST);
      // block->describe_instance('-');
      // block->data.describe_instance('-');

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
      printf("PRINTING THE FREE LIST @ 0x%lx = %ul!\n", &FREE_LIST, &FREE_LIST);
      LINE;

      size_t ix {0};
      for (const auto & x : FREE_LIST_HEAD) {
        printf("Node                : #%u\n", ++ix);
        PRINT("Node is at", &x);
        PROFFSET(&x);
        PRINT("With block start at", &x.data.block_start);
        PROFFSET(&x.data.block_start);

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
      printf("ALLOCATING MEMORY FROM THE FREE LIST @ 0x%lx = %ul!\n", &FREE_LIST, &FREE_LIST);
      LINE;
      PRINT("Bytes requested: ", size);
      
      void *       pointer {nullptr};
      alloc_node * block   {nullptr};

      assert(size > 0);
      
      // Align the pointer
      size = align_up(size, sizeof(void *));

      // try to find a big enough block to alloc
      for (auto & b : FREE_LIST)
        if (b.data.size >= size)
        {
          block = &b;
          pointer = &b.data.block_start;

          PRINT("Selected block at", &b);
          PROFFSET(&b);
          PRINT("With block start at", &b.data.block_start);
          PROFFSET(&b.data.block_start);
          HLINE;
          
          block->describe_instance();
          block->data.describe_instance('-');
          LINE;
          
          break;
        }

      // PRINT("Selected block at", uintptr(&block->data.block_start));
      
      if (nullptr == block)
        return pointer;

      // Can we split the blocko?
      if ((block->data.size - size) >= MIN_ALLOC_SZ)
      {
        alloc_node * new_block;
        new_block = reinterpret_cast<alloc_node *>((uintptr(pointer) + size));
        
        new_block->data.size = block->data.size - size - ALLOC_HEADER_SZ;
        block->data.size = size;

        new_block->insert_before(block);

        block->remove();
        
        PRINT("Created new block at", new_block);
        PROFFSET(new_block);
        PRINT("With block start at", &new_block->data.block_start);
        PROFFSET(&new_block->data.block_start);

        HLINE;
        new_block->describe_instance();
        new_block->data.describe_instance();
        HLINE;
        //   list_add_(&new_block->node, &block->node, block->node.next);
      }
      else {
        printf(
          "SUSPICIOUS ALLOC: not %zu - %zu = %zu >= %zu.\n",
          (block->data.size),
          size,
          (block->data.size - size),
          MIN_ALLOC_SZ);
      }
      
      // list_del(&block->node);

      PRINT("Gave pointer to", uintptr(pointer));
      LINE;
      
      return pointer;
    }

    // ===========================================================================================================

    void defrag();
    
    void release(void * pointer) {

      assert(nullptr != pointer);

      LINE;
      printf("RELEASING 0x%lx = %ul!\n", uintptr(pointer));
      LINE;

      alloc_node * block;
      // alloc_node * free_block;

      block = reinterpret_cast<alloc_node *>(uintptr(pointer) - ALLOC_HEADER_SZ);
      PRINT("It's node is", block);
      LINE;
      putchar('\n');;
      
      // Let's put it back in the proper spot
      // list_for_each_entry(free_block, &free_list, node)
      // {
      //   if(free_block > block)
      //   {
      //     list_add_(&block->node, free_block->node.prev, &free_block->node);
      //     goto blockadded;
      //   }
      // }
      // list_add_tail(&block->node, &free_list);
 
      for (auto & free_block : FREE_LIST_HEAD) {
        if (&free_block > block) {
          block->insert_before(free_block);
          goto blockadded;
        }
      }

      block->insert_after(FREE_LIST);;
      
    blockadded:
      // Let's see if we can combine any memory
      defrag();
    }

    // ===========================================================================================================

    void defrag() {
      LINE;
      printf("DEFRAGMMENTING THE FREE LIST @ 0x%lx = %ul!\n", &FREE_LIST, &FREE_LIST);
      LINE;

      // alloc_node * block      {nullptr};
      alloc_node * last_block {nullptr};
      // alloc_node * t          {nullptr};

      for (auto & block : FREE_LIST_HEAD) {
        if ((nullptr != last_block)
            && (uintptr(&last_block->data.block_start) + last_block->data.size) == uintptr(&block))
        {
          last_block->data.size += ALLOC_HEADER_SZ + block.data.size;
          printf("Removing this block:.\n");
          block.describe_instance();
          block.data.describe_instance();
          block.remove();
          continue;
        }
        
        last_block = &block;
      }

      HLINE;
      printf("Done defragmenting.\n");
      LINE;
      putchar('\n');
    }
  
    // =========================================================================================================
    
  }
}

#undef PRINT
#undef PROFFSET
#undef PRINTWOFFSET
#undef LINE
#undef HLINE
#undef FREE_LIST_HEAD
  
#endif
