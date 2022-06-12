#ifndef RESEUNE_BASE_ONE_ALLOCATOR_HPP
#define RESEUNE_BASE_ONE_ALLOCATOR_HPP

#include <inttypes.h>
#include <stdint.h>
#include <cassert>

#include "reseune/util/util.hpp"
#include "reseune/alloc_info/alloc_info.hpp"
#include "reseune/doubly_linked/doubly_linked.hpp"

#include "macros.hpp"

namespace reseune {
  namespace base_one {
    // ===========================================================================================================
#ifdef RESEUNE_SINGLETON_ALLOCATOR
    namespace allocator {
#else
    class allocator {
    public:
#endif
      
      using alloc_node = doubly_linked<alloc_info>;    

#ifdef RESEUNE_SINGLETON_ALLOCATOR
      static  alloc_node   FREE_LIST {nullptr, nullptr};
#else
      alloc_node FREE_LIST;

      // ===========================================================================================================

      constexpr allocator() : FREE_LIST {nullptr, nullptr } {}
#endif
      
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
        RCONSP(pblock, FREE_LIST);
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
        const bool verbose {true};

        ASSERTISNOTNULL(PFREE_LIST_HEAD);
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
            PRINT("With block start at", pvoid);
            PRHLINE;
            DESCRIBEP(pblock);
            PRLINE;
          
            break;
          }

        IFISNULL(pblock) {
          WARN("OUT OF MEMORY IN FREE LIST @ 0x%lx = %ul!\n", PFREE_LIST, PFREE_LIST);        

          return nullptr;
        }

        alloc_node & block {*pblock};
      
        // Check if we can we split the block:
        if ((BSIZE(block) - size) >= MIN_ALLOC_SZ) {
          palloc_node pnew_block {PALLOC_NODE((UINTPTR(pvoid) + size))};
        
          SETBSIZEP(pnew_block, BSIZE(block) - size - ALLOC_HEADER_SZ); // What happens when this is 0?
          SETBSIZE(block, size);
          RCONSP(pnew_block, block);
          REMOVE(block);

          PRINT("Created new block at", pnew_block);
          PRINT("With block start at", BSTARTP(pnew_block));
          PRHLINE;
          DESCRIBEP(pnew_block);
          PRHLINE;
        }
#ifndef NDEBUG
        else {
          WARN(
            "SUSPICIOUS ALLOC: not %zu - %zu = %zu >= %zu.\n",
            BSIZE(block),
            size,
            (BSIZE(block) - size),
            MIN_ALLOC_SZ);
        
          assert(false);
        }
#endif
      
        PRINT("Gave pointer to", UINTPTR(pvoid));
        PRLINE;
      
        return pvoid;
      }

      // ===========================================================================================================

      template <typename T>
#ifdef RESEUNE_SINGLETON_ALLOCATOR
      static
#endif
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
        palloc_node released_pblock {PALLOC_NODE(UINTPTR(addr) - ALLOC_HEADER_SZ)};

        ASSERTISNOTNULL(addr);
        PRLINE;
        PRINTF("RELEASING 0x%lx = %ul!\n", UINTPTR(addr));
        PRLINE;
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
    };
  }
}

#include "undef_macros.hpp"  
#endif
