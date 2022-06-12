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

#define                    PALLOC_NODE(x)       (reinterpret_cast<palloc_node>(x))
#define                    ASSERTISNOTNULL(x)   assert(ISNOTNULL(x))
#define                    BSIZE(b)             (b.data.size)
#define                    BSIZEP(pb)           (BSIZE((*bp)))
#define                    CONS(head, tail)     head.insert_before(tail)
#define                    CONSP(headp, tail)   CONS((*headp), tail)
#define                    DESCRIBE(block)      { if (verbose) { block.describe_instance(); block.data.describe_instance(); } }
#define                    DESCRIBEP(pblock)    { DESCRIBE((*pblock)); }
#define                    IFISNOTNULL(x)       if (ISNOTNULL(x))
#define                    IFISNULL(x)          if (ISNULL(x))
#define                    ISNOTNULL(x)         (nullptr != x)
#define                    ISNULL(x)            (nullptr == x)
#define                    PRINT(x, y)          { if (verbose) print_bits<true, false>((x), UINTPTR(y)); }
#define                    PRINTF(...)          { if (verbose) printf(__VA_ARGS__); }
#define                    PROFFSET(x)          { PROFFSETP(&x); }
#define                    PROFFSETP(x)         { PRINT("... with offset", UINTPTR(x) - UINTPTR(MEMORY)); }
#define                    PUTCHAR(c)           { if (verbose) putchar(c); }
#define                    REMOVE(b)            b.remove()
#define                    REMOVEP(b)           REMOVE(*b)
#define                    RPLACD(list, tail)   tail.insert_after(list)
#define                    RPLACDP(list, ptail) RPLACD(list, (*ptail))
#define                    SETBSIZE(b, s)       b.data.size = s
#define                    SETBSIZEP(pb, s)     SETBSIZE((*pb), s)
#define                    UINTPTR(x)           (uintptr(x))
#define                    ALLOC_HEADER_SZ      (offsetof(alloc_node, data.block_start))
#define                    FOR_EACH_BLOCK       for (auto & block : FREE_LIST_HEAD)
#define                    FREE_LIST_HEAD       (*PFREE_LIST_HEAD)
#define                    HLINE                { if (verbose) print_line('-'); }
#define                    LINE                 { if (verbose) print_line(); }
#define                    MIN_ALLOC_SZ         (ALLOC_HEADER_SZ + 32)
#define                    PFREE_LIST_HEAD      (FREE_LIST.next)
#define                    VERBOSEARG           bool verbose = false
#define                    VOID                 inline void 
#define                    PVOID                void * 
#define                    palloc_node          alloc_node *
    using                  alloc_node         = doubly_linked<alloc_info>;    
    constexpr size_t       MEMORY_WORDS         {1024};
    constexpr size_t       MEMORY_BYTES         {MEMORY_WORDS << 3};
    static    char         MEMORY[MEMORY_BYTES] {0};
    static    alloc_node   FREE_LIST            {nullptr, nullptr};
    
    // ===========================================================================================================

    VOID alloc_add_block(PVOID const addr, size_t size, VERBOSEARG) {
      LINE;
      PRINTF("ADDING NEW MEMORY TO THE FREE LIST @ 0x%lx = %ul!\n", &FREE_LIST, &FREE_LIST);
      LINE;
      PRINT("Given memory at", addr);
      PRINT("Given bytes", size);
      
      // align the start addr of our pblock to the next pointer aligned addr
      palloc_node pblock {PALLOC_NODE(align_up(UINTPTR(addr), sizeof(PVOID)))};
        
      PRINT("Aligned pblock to", pblock);

      // calculate actual size - overhead
      SETBSIZEP(pblock, 
                UINTPTR(addr)
                + size
                - UINTPTR(pblock)
                - ALLOC_HEADER_SZ);

      RPLACDP(FREE_LIST, pblock);

      LINE;
      PUTCHAR('\n');      
    }

    // ===========================================================================================================

    VOID initialize(VERBOSEARG) {
      alloc_add_block(MEMORY, MEMORY_BYTES, verbose);
    }

    // ===========================================================================================================
    
    VOID describe_free_list() {
      ASSERTISNOTNULL(PFREE_LIST_HEAD);

      const bool verbose {true};
            
      LINE;
      PRINTF("PRINTING THE FREE LIST @ 0x%lx = %ul!\n", &FREE_LIST, &FREE_LIST);
      LINE;

      size_t ix {0};
      
      FOR_EACH_BLOCK {
        PRINTF("Node                : #%u\n", ++ix);
        PRINT("Node is at", &block);
        PROFFSET(block);
        PRINT("With block start at", &block.data.block_start);
        PROFFSET(block.data.block_start);
        HLINE;
        DESCRIBE(block);
        LINE;
      }
  
      PUTCHAR('\n');
    }

    // ===========================================================================================================
    
    inline PVOID alloc(size_t size, VERBOSEARG) {
      assert(size > 0);
      
      size = align_up(size, sizeof(PVOID)); // Align the pointer

      LINE;
      PRINTF("ALLOCATING MEMORY FROM THE FREE LIST @ 0x%lx = %ul!\n", &FREE_LIST, &FREE_LIST);
      LINE;
      PRINT("Bytes requested: ", size);
      
      PVOID       pvoid  {nullptr};
      palloc_node pblock {nullptr};

      // try to find a big enough block to alloc
      FOR_EACH_BLOCK
        if (block.data.size >= size)
        {
          pblock = &block;
          pvoid  = &block.data.block_start;

          PRINT("Selected block at", pblock);
          PROFFSETP(pblock);
          PRINT("With block start at", &block.data.block_start);
          PROFFSET(block.data.block_start);
          HLINE;
          DESCRIBEP(pblock);
          LINE;
          
          break;
        }

      IFISNULL(pblock)
        return pvoid;

      alloc_node & block {*pblock};
      
      // Can we split the block?
      if ((block.data.size - size) >= MIN_ALLOC_SZ) {
        palloc_node pnew_block {PALLOC_NODE((UINTPTR(pvoid) + size))};
        
        SETBSIZEP(pnew_block, block.data.size - size - ALLOC_HEADER_SZ);
        SETBSIZE(block, size);
        CONSP(pnew_block, block);
        REMOVE(block);
        
        PRINT("Created new block at", pnew_block);
        PROFFSETP(pnew_block);
        PRINT("With block start at", &pnew_block->data.block_start);
        PROFFSET(pnew_block->data.block_start);

        HLINE;
        DESCRIBEP(pnew_block);
        HLINE;
      }
      else {
        PRINTF(
          "SUSPICIOUS ALLOC: not %zu - %zu = %zu >= %zu.\n",
          (block.data.size),
          size,
          (block.data.size - size),
          MIN_ALLOC_SZ);
        
        assert(false);
      }

      PRINT("Gave pointer to", UINTPTR(pvoid));
      LINE;
      
      return pvoid;
    }

    // ===========================================================================================================

    VOID defragment(bool varbose);
    
    VOID release(PVOID const pointer, VERBOSEARG) {
      ASSERTISNOTNULL(pointer);

      LINE;
      PRINTF("RELEASING 0x%lx = %ul!\n", UINTPTR(pointer));
      LINE;

      palloc_node released_pblock {PALLOC_NODE(UINTPTR(pointer) - ALLOC_HEADER_SZ)};
      
      PRINT("It's node is at", released_pblock);
      LINE;
      PUTCHAR('\n');
      
      // Let's put it back in the proper spot
      FOR_EACH_BLOCK
        if (&block > released_pblock) {
          CONSP(released_pblock, block);

          goto block_added;
        }

      CONSP(released_pblock, FREE_LIST_HEAD);
      
    block_added:
      // Let's see if we can combine any memory
      defragment(verbose);
    }

    // ===========================================================================================================

    VOID defragment(VERBOSEARG) {
      LINE;
      PRINTF("DEFRAGMENTMMENTING THE FREE LIST @ 0x%lx = %ul!\n", &FREE_LIST, &FREE_LIST);
      LINE;

      palloc_node plast_block {nullptr};

      FOR_EACH_BLOCK {
        IFISNOTNULL(plast_block) 
          if ((UINTPTR(&plast_block->data.block_start) + plast_block->data.size) == UINTPTR(&block)) {
            plast_block->data.size += ALLOC_HEADER_SZ + block.data.size;

            PRINTF("Removing this block:.\n");
            DESCRIBE(block);
            REMOVE(block);

            // continue; // this seems unnecessary?
          }
        
        plast_block = &block;
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
#undef PALLOC_NODE
#undef ASSERTISNOTNULL
#undef CONS
#undef CONSP
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
#undef PROFFSETP
#undef PUTCHAR
#undef PVOID
#undef REMOVE
#undef REMOVEP
#undef RPLACD
#undef RPLACDP
#undef UINTPTR
#undef VERBOSEARG
#undef VOID
#undef palloc_node

#endif
