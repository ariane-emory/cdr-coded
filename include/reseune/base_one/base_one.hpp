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

#define                    ALLOC_HEADER_SZ      (offsetof(alloc_node, data.block_start))
#define                    ALLOC_NODEP(x)       (reinterpret_cast<alloc_nodep>(x))
#define                    ASSERTISNOTNULL(x)   assert(ISNOTNULL(x))
#define                    DESCRIBE(block)      { if (verbose) { block.describe_instance(); block.data.describe_instance(); } }
#define                    DESCRIBEP(blockp)    { DESCRIBE((*blockp)); }
#define                    FOR_EACH_BLOCK       for (auto & block : FREE_LIST_HEAD)
#define                    FREE_LIST_HEAD       (*FREE_LIST_HEADP)
#define                    FREE_LIST_HEADP      (FREE_LIST.next)
#define                    HLINE                { if (verbose) print_line('-'); }
#define                    IFISNOTNULL(x)       if (ISNOTNULL(x))
#define                    IFISNULL(x)          if (ISNULL(x))
#define                    ISNOTNULL(x)         (nullptr != x)
#define                    ISNULL(x)            (nullptr == x)
#define                    LINE                 { if (verbose) print_line(); }
#define                    MIN_ALLOC_SZ         (ALLOC_HEADER_SZ + 32)
#define                    PRINT(x,y)           { if (verbose) print_bits<true, false>((x), UINTPTR(y)); }
#define                    PRINTF(...)          { if (verbose) printf(__VA_ARGS__); }
#define                    PROFFSET(x)          { PRINT("... with offset", UINTPTR(x) - UINTPTR(MEMORY)); }
#define                    PUTCHAR(c)           { if (verbose) putchar(c); }
#define                    UINTPTR(x)           (uintptr(x))
#define                    VERBOSEARG           bool verbose = false
#define                    VOIDP                void * 
#define                    alloc_nodep          alloc_node *
    using                  alloc_node         = doubly_linked<alloc_info>;    
    constexpr size_t       MEMORY_WORDS         {1024};
    constexpr size_t       MEMORY_BYTES         {MEMORY_WORDS << 3};
    static    char         MEMORY[MEMORY_BYTES] {0};
    static    alloc_node   FREE_LIST            {nullptr, nullptr};
    
    // ===========================================================================================================

    inline void alloc_add_block(VOIDP const addr, size_t size, VERBOSEARG) {
      LINE;
      PRINTF("ADDING NEW MEMORY TO THE FREE LIST @ 0x%lx = %ul!\n", &FREE_LIST, &FREE_LIST);
      LINE;
      PRINT("Given memory at", addr);
      PRINT("Given bytes", size);
      
      // align the start addr of our blockp to the next pointer aligned addr
      alloc_nodep blockp {ALLOC_NODEP(align_up(UINTPTR(addr), sizeof(VOIDP)))};
        
      PRINT("Aligned blockp to", blockp);

      // calculate actual size - overhead
      blockp->data.size =
        UINTPTR(addr)
        + size
        - UINTPTR(blockp)
        - ALLOC_HEADER_SZ;
      
      blockp->insert_after(FREE_LIST);

      LINE;
      PUTCHAR('\n');      
    }

    // ===========================================================================================================

    inline void initialize(VERBOSEARG) {
      alloc_add_block(MEMORY, MEMORY_BYTES, verbose);
    }

    // ===========================================================================================================
    
    inline void describe_free_list() {
      const bool verbose {true};
      
      ASSERTISNOTNULL(FREE_LIST_HEADP);
      
      LINE;
      PRINTF("PRINTING THE FREE LIST @ 0x%lx = %ul!\n", &FREE_LIST, &FREE_LIST);
      LINE;

      size_t ix {0};
      
      FOR_EACH_BLOCK {
        PRINTF("Node                : #%u\n", ++ix);
        PRINT("Node is at", &block);
        PROFFSET(&block);
        PRINT("With block start at", &block.data.block_start);
        PROFFSET(&block.data.block_start);
        HLINE;
        DESCRIBE(block);
        LINE;
      }
  
      PUTCHAR('\n');
    }

    // ===========================================================================================================
    
    inline VOIDP alloc(size_t size, VERBOSEARG) {
      LINE;
      PRINTF("ALLOCATING MEMORY FROM THE FREE LIST @ 0x%lx = %ul!\n", &FREE_LIST, &FREE_LIST);
      LINE;
      PRINT("Bytes requested: ", size);
      
      VOIDP       pointer {nullptr};
      alloc_nodep blockp  {nullptr};

      assert(size > 0);
      
      // Align the pointer
      size = align_up(size, sizeof(VOIDP));

      // try to find a big enough block to alloc
      FOR_EACH_BLOCK
        if (block.data.size >= size)
        {
          blockp  = &block;
          pointer = &block.data.block_start;

          PRINT("Selected block at", blockp);
          PROFFSET(blockp);
          PRINT("With block start at", &block.data.block_start);
          PROFFSET(&block.data.block_start);
          HLINE;
          DESCRIBEP(blockp);
          LINE;
          
          break;
        }

      IFISNULL(blockp)
        return pointer;

      // Can we split the block?
      if ((blockp->data.size - size) >= MIN_ALLOC_SZ) {
        alloc_nodep new_blockp { ALLOC_NODEP((UINTPTR(pointer) + size)) };
        
        new_blockp->data.size = blockp->data.size - size - ALLOC_HEADER_SZ;
        blockp    ->data.size = size;
        new_blockp->insert_before(blockp);
        blockp    ->remove();
        
        PRINT("Created new block at", new_blockp);
        PROFFSET(new_blockp);
        PRINT("With block start at", &new_blockp->data.block_start);
        PROFFSET(&new_blockp->data.block_start);

        HLINE;
        DESCRIBEP(new_blockp);
        HLINE;
      }
      else {
        PRINTF(
          "SUSPICIOUS ALLOC: not %zu - %zu = %zu >= %zu.\n",
          (blockp->data.size),
          size,
          (blockp->data.size - size),
          MIN_ALLOC_SZ);
      }

      PRINT("Gave pointer to", UINTPTR(pointer));
      LINE;
      
      return pointer;
    }

    // ===========================================================================================================

    inline void defragment(bool varbose);
    
    inline void release(VOIDP pointer, VERBOSEARG) {
      ASSERTISNOTNULL(pointer);

      LINE;
      PRINTF("RELEASING 0x%lx = %ul!\n", UINTPTR(pointer));
      LINE;

      alloc_nodep released_blockp {ALLOC_NODEP(UINTPTR(pointer) - ALLOC_HEADER_SZ)};
      
      PRINT("It's node is at", released_blockp);
      LINE;
      PUTCHAR('\n');
      
      // Let's put it back in the proper spot
      FOR_EACH_BLOCK
        if (&block > released_blockp) {
          released_blockp->insert_before(block);

          goto block_added;
        }

      // released_blockp->insert_after(FREE_LIST);
      released_blockp->insert_before(FREE_LIST_HEAD);
      
    block_added:
      // Let's see if we can combine any memory
      defragment(verbose);
    }

    // ===========================================================================================================

    inline void defragment(VERBOSEARG) {
      LINE;
      PRINTF("DEFRAGMENTMMENTING THE FREE LIST @ 0x%lx = %ul!\n", &FREE_LIST, &FREE_LIST);
      LINE;

      alloc_nodep last_blockp {nullptr};

      FOR_EACH_BLOCK {
        IFISNOTNULL(last_blockp) 
          if ((UINTPTR(&last_blockp->data.block_start) + last_blockp->data.size) == UINTPTR(&block)) {
            last_blockp->data.size += ALLOC_HEADER_SZ + block.data.size;

            PRINTF("Removing this block:.\n");
            DESCRIBE(block);

            block.remove();

            // continue; // this seems unnecessary?
          }
        
        last_blockp = &block;
      }

      HLINE;
      PRINTF("Done defragmenting.\n");
      LINE;
      PUTCHAR('\n');
    }
  
    // =========================================================================================================
    
  }
}

#undef ALLOC_HEADER_SZ
#undef ALLOC_NODEP
#undef ASSERTISNOTNULL
#undef DESCRIBE
#undef DESCRIBEP
#undef FOR_EACH_BLOCK
#undef FREE_LIST_HEAD
#undef HLINE
#undef IFISNOTNULL
#undef IFISNULL
#undef ISNOTNULL
#undef ISNULL
#undef LINE
#undef MIN_ALLOC_SZ
#undef PRINT
#undef PRINTF
#undef PROFFSET
#undef PUTCHAR
#undef UINTPTR
#undef VERBOSEARG
#undef VOIDP
#undef alloc_nodep

#endif
