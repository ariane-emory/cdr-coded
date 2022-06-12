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

#define                    ASSERTISNOTNULL(x)   assert(ISNOTNULL(x))
#define                    BSIZE(b)             (b.data.size)
#define                    BSIZEP(pb)           (BSIZE((*pb)))
#define                    BSTART(b)            (&b.data.block_start)
#define                    BSTARTP(pb)          (BSTART((*pb)))
#define                    CONS(head, list)     head.insert_before(list)
#define                    CONSP(headp, list)   CONS((*headp), list)
#define                    DESCRIBE(block)      { if (verbose) { block.describe_instance(); block.data.describe_instance(); } }
#define                    DESCRIBEP(pblock)    { DESCRIBE((*pblock)); }
#define                    IFISNOTNULL(x)       if (ISNOTNULL(x))
#define                    IFISNULL(x)          if (ISNULL(x))
#define                    ISNOTNULL(x)         (nullptr != x)
#define                    ISNULL(x)            (nullptr == x)
#define                    PALLOC_NODE(x)       (reinterpret_cast<palloc_node>(x))
#define                    PRINT(x, y)          { if (verbose) print_bits<true, false>((x), UINTPTR(y)); }
#define                    PRINTF(...)          { if (verbose) WARN(__VA_ARGS__); }
#define                    PUTCHAR(c)           { if (verbose) putchar(c); }
#define                    PVOIDFUN(name, ...)  inline void * name(__VA_ARGS__)
#define                    RCONS(list, tail)    tail.insert_after(list)
#define                    RCONSP(list, ptail)  RCONS(list, (*ptail))
#define                    REMOVE(b)            b.remove()
#define                    REMOVEP(b)           REMOVE(*b)
#define                    SETBSIZE(b, s)       b.data.size = s
#define                    SETBSIZEP(pb, s)     SETBSIZE((*pb), s)
#define                    UINTPTR(x)           (uintptr(x))
#define                    VOIDFUN(name, ...)   inline void name(__VA_ARGS__)
#define                    WARN(...)            { printf(__VA_ARGS__); }
// This offset will only print correctly for locations in MEMORY:
#define                    ADDRARG              PVOIDC addr
#define                    ALLOC_HEADER_SZ      (offsetof(alloc_node, data.block_start))
#define                    FOR_EACH_BLOCK       for (auto & block : FREE_LIST_HEAD)
#define                    FREE_LIST_HEAD       (*PFREE_LIST_HEAD)
#define                    MIN_ALLOC_SZ         (ALLOC_HEADER_SZ + 32)
#define                    PFREE_LIST           (&FREE_LIST)
#define                    PFREE_LIST_HEAD      (FREE_LIST.next)
#define                    PRHLINE              { if (verbose) print_line('-'); }
#define                    PRLINE               { if (verbose) print_line(); }
#define                    PVOID                void *
#define                    PVOIDC               PVOID const 
#define                    SIZEARG              size_t size
#define                    VERBOSEARG           bool verbose = false
#define                    palloc_node          alloc_node *
#ifndef RESEUNE_NO_BASE_ONE_MEMORY
    constexpr size_t       MEMORY_WORDS         {1024};
    constexpr size_t       MEMORY_BYTES         {MEMORY_WORDS << 3};
    static    char         MEMORY[MEMORY_BYTES] {0};
#endif
    using                  alloc_node         = doubly_linked<alloc_info>;    
    static    alloc_node   FREE_LIST            {nullptr, nullptr};
    
    // ===========================================================================================================

    
    VOIDFUN(add_memory, ADDRARG, SIZEARG, VERBOSEARG) {
      PRLINE;
      PRINTF("ADDING NEW MEMORY TO THE FREE LIST @ 0x%lx = %ul!\n", PFREE_LIST, PFREE_LIST);
      PRLINE;
      PRINT("Given memory at", addr);
      PRINT("Given bytes", size);
      
      // align the start addr of our pblock to the next pointer aligned addr
      palloc_node pblock {PALLOC_NODE(align_up(UINTPTR(addr), sizeof(PVOID)))};
        
      PRINT("Aligned pblock to", pblock);

      // calculate actual size - overhead
      SETBSIZEP(
        pblock, 
        UINTPTR(addr)
        + size
        - UINTPTR(pblock)
        - ALLOC_HEADER_SZ);

      RCONSP(FREE_LIST, pblock);

      PRLINE;
      PUTCHAR('\n');      
    }

    // ===========================================================================================================

    VOIDFUN(initialize, VERBOSEARG) {
#ifndef RESEUNE_NO_BASE_ONE_MEMORY
      add_memory(MEMORY, MEMORY_BYTES, verbose);
#else
      WARN("initialize must not be called when -DRESEUNE_NO_BASE_ONE_MEMORY.");
      assert(false);
#endif
    }
    
    // ===========================================================================================================
    
    VOIDFUN(describe_free_list) {
      ASSERTISNOTNULL(PFREE_LIST_HEAD);

      const bool verbose {true};
            
      PRLINE;
      PRINTF("PRINTING THE FREE LIST @ 0x%lx = %ul!\n", PFREE_LIST, PFREE_LIST);
      PRLINE;

      size_t ix {0};
      
      FOR_EACH_BLOCK {
        PRINTF("Node                : #%u\n", ++ix);
        PRINT("Node is at", &block);
        PRINT("With block start at", BSTART(block));
        PRHLINE;
        DESCRIBE(block);
        PRLINE;
      }
  
      PUTCHAR('\n');
    }

    // ===========================================================================================================
    
    PVOIDFUN(valloc, SIZEARG, size_t each, VERBOSEARG) {
      return valloc(size * each, verbose);
    }
    
    // ===========================================================================================================
    
    PVOIDFUN(valloc, SIZEARG, VERBOSEARG) {
      assert(size > 0);
      
      size = align_up(size, sizeof(PVOID)); // Align the pointer

      PRLINE;
      PRINTF("ALLOCATING MEMORY FROM THE FREE LIST @ 0x%lx = %ul!\n", PFREE_LIST, PFREE_LIST);
      PRLINE;
      PRINT("Bytes requested: ", size);
      
      PVOID       pvoid  {nullptr};
      palloc_node pblock {nullptr};

      // try to find a big enough block to alloc
      FOR_EACH_BLOCK
        if (BSIZE(block) >= size)
        {
          pblock = &block;
          pvoid  = BSTART(block);

          PRINT("Selected block at", pblock);
          PRINT("With block start at", BSTART(block));
          PRHLINE;
          DESCRIBEP(pblock);
          PRLINE;
          
          break;
        }

      IFISNULL(pblock) {
        WARN("OUT OF MEMORY IN FREE LIST @ 0x%lx = %ul!\n", PFREE_LIST, PFREE_LIST);
        
        return pvoid;
      }

      alloc_node & block {*pblock};
      
      // Can we split the block?
      if ((BSIZE(block) - size) >= MIN_ALLOC_SZ) {
        palloc_node pnew_block {PALLOC_NODE((UINTPTR(pvoid) + size))};
        
        SETBSIZEP(pnew_block, BSIZE(block) - size - ALLOC_HEADER_SZ);
        SETBSIZE(block, size);
        CONSP(pnew_block, block);
        REMOVE(block);
        
        PRINT("Created new block at", pnew_block);
        PRINT("With block start at", BSTARTP(pnew_block));

        PRHLINE;
        DESCRIBEP(pnew_block);
        PRHLINE;
      }
      else {
        WARN(
          "SUSPICIOUS ALLOC: not %zu - %zu = %zu >= %zu.\n",
          BSIZE(block),
          size,
          (BSIZE(block) - size),
          MIN_ALLOC_SZ);
        
        assert(false);
      }

      PRINT("Gave pointer to", UINTPTR(pvoid));
      PRLINE;
      
      return pvoid;
    }

    // ===========================================================================================================

    template <typename T>
    inline T * alloc(SIZEARG, VERBOSEARG) {
      return reinterpret_cast<T *>(valloc(size * sizeof(T), verbose));
    }
    
    // ===========================================================================================================

    VOIDFUN(defragment, VERBOSEARG) {
      PRLINE;
      PRINTF("DEFRAGMENTMMENTING THE FREE LIST @ 0x%lx = %ul!\n", PFREE_LIST, PFREE_LIST);
      PRLINE;

      palloc_node plast_block {nullptr};

      FOR_EACH_BLOCK {
        IFISNOTNULL(plast_block) 
          if ((UINTPTR(BSTARTP(plast_block)) + BSIZEP(plast_block)) == UINTPTR(&block)) {
            SETBSIZEP(plast_block, BSIZEP(plast_block) + ALLOC_HEADER_SZ + BSIZE(block));

            PRINTF("Removing this block:.\n");
            DESCRIBE(block);
            REMOVE(block);

            // continue; // this seems unnecessary?
          }
        
        plast_block = &block;
      }

      PRHLINE;
      PRINTF("Done defragmenting.\n");
      PRLINE;
      PUTCHAR('\n');
    }
  
    // ===========================================================================================================
    
    VOIDFUN(release, ADDRARG, VERBOSEARG) {
      ASSERTISNOTNULL(addr);

      PRLINE;
      PRINTF("RELEASING 0x%lx = %ul!\n", UINTPTR(addr));
      PRLINE;

      palloc_node released_pblock {PALLOC_NODE(UINTPTR(addr) - ALLOC_HEADER_SZ)};
      
      PRINT("It's node is at", released_pblock);
      PRLINE;
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

    // =========================================================================================================
    
  }
}

#undef ADDRARG
#undef ALLOC_HEADER_SZ
#undef ASSERTISNOTNULL
#undef BSIZE
#undef BSIZEP
#undef BSTART
#undef BSTARTP
#undef CONS
#undef CONSP
#undef DESCRIBE
#undef DESCRIBEP
#undef FOR_EACH_BLOCK
#undef FREE_LIST_HEAD
#undef IFISNOTNULL
#undef IFISNULL
#undef ISNOTNULL
#undef ISNULL
#undef MIN_ALLOC_SZ
#undef PALLOC_NODE
#undef PFREE_LIST
#undef PFREE_LIST_HEAD
#undef PRHLINE
#undef PRINT
#undef PRINTF
#undef PRLINE
#undef PUTCHAR
#undef PVOID
#undef PVOIDC
#undef PVOIDFUN
#undef RCONS
#undef RCONSP
#undef REMOVE
#undef REMOVEP
#undef SETBSIZE
#undef SETBSIZEP
#undef SIZEARG
#undef UINTPTR
#undef VERBOSEARG
#undef VOIDFUN
#undef palloc_node

#endif
