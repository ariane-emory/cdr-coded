#ifndef RESEUNE_ALLOCATOR_HPP
#define RESEUNE_ALLOCATOR_HPP

#include <inttypes.h>
#include <stdint.h>
#include <cassert>

#include "reseune/util/util.hpp"
#include "reseune/alloc_info/alloc_info.hpp"
#include "reseune/doubly_linked/doubly_linked.hpp"

#include "macros.hpp"

namespace reseune {
  // ===========================================================================================================

  template <typename T>
  class allocator {
  public:
    using alloc_info = T;
    using alloc_node = doubly_linked<alloc_info>;

#ifdef RESEUNE_SINGLETON_ALLOCATOR
    static alloc_node root;
  private:
    allocator() {};
  public:
#else
    alloc_node root;
#endif
      
    // =======================================================================================================

    struct strategies {
      template <typename block_type>
         struct do_not_track_used {
        static inline void commit_block(block_type & block) {
          block.remove();
        }
    
        static inline void release_block(block_type & block, VERBOSEARG) {
          place_block(block, verbose);
        }
    
        static inline bool block_is_free(block_type const & block) {
          return true;
        }
      };
    };
    
    // =======================================================================================================
    
  private:

    // =======================================================================================================

    VOIDFUN(split_blockp, palloc_node pblock, SIZEARG, VERBOSEARG) {
      alloc_node & block      {*pblock};
      alloc_node & new_block  {*PALLOC_NODE((UINTPTR(BSTART(block)) + size))};
        
      SETBSIZE(new_block, BSIZE(block) - size - ALLOC_HEADER_SZ); // What happens when this is 0?
      SETBSIZE(block, size);
      RCONS(new_block, block);
      // REMOVE(block);
      
      strategies::do_not_track_used_block(block);

      PRINT("Created new block at", &new_block);
      PRINT("With block start at", BSTART(new_block));
      PRHLINE;
      DESCRIBE(new_block);
      PRHLINE;
    }

    VOIDFUN(place_block, alloc_node & pnew_block, VERBOSEARG) {
      place_blockp(&pnew_block, verbose);
    }
    
    VOIDFUN(place_blockp, palloc_node pnew_block, VERBOSEARG) {
      PRLINE;
      PRINT("Placing block", pnew_block);
 
      IFISNULL(PFREE_LIST_HEAD) {
        PRINTF("Placing after FL.\n");
      
        RCONSP(pnew_block, root);

        return;
      } 

      palloc_node last_block {nullptr}; 
 
      FOR_EACH_BLOCK { 
        last_block = &block; 

        PRINT("Compare with", uintptr(&block));
              
        if (last_block <= pnew_block) 
          continue;
                
        PRINTF("Placed block is before this block.\n"); 

        CONSP(pnew_block, block); 

        return;
      } 

      // There shouldn't be any way for last_block to be null if we got this far.
              
      PRINTF("Placed block is at the end.\n");
              
      RCONSP(pnew_block, last_block); 
    }
          
    // =======================================================================================================

  public:

    // =======================================================================================================
        
    VOIDFUN(add_memory, ADDRARG, SIZEARG, VERBOSEARG) {
      PRLINE;
      PRINTF("ADDING NEW MEMORY TO THE FREE LIST @ 0x%lx = %ul!\n", PROOT, PROOT);
      PRLINE;
      PRINT("Given memory at", addr);
      PRINT("Given bytes", size);
      
      ASSERTISNOTNULL(addr);

      // align the start addr of our pnew_block to the next pointer aligned addr
      palloc_node  pnew_block {PALLOC_NODE(align_up(UINTPTR(addr), sizeof(PVOID)))};
          
      PRINT("Align pnew_block to", pnew_block);
          
      // calculate actual size - overhead
      SETBSIZEP(
        pnew_block, 
        UINTPTR(addr)
        + size
        - UINTPTR(pnew_block)
        - ALLOC_HEADER_SZ);
            
      place_blockp(pnew_block, verbose);
                    
      PRLINE;
      PRNL;
    }

    // =======================================================================================================
    
    VOIDFUN(describe_free_list) {
      const bool verbose {true};

      PRLINE;
      PRINTF("PRINTING THE FREE LIST @ 0x%lx = %ul!\n", PROOT, PROOT);
      PRLINE;

      ASSERTISNOTNULL(PFREE_LIST_HEAD);

      size_t ix {0};
      
      FOR_EACH_BLOCK {
        PRINTF("Node                : #%u\n", ++ix);
        PRINT("Node is at", &block);
        PRINT("With block start at", BSTART(block));
        PRHLINE;
        DESCRIBE(block);
        PRLINE;
      }
  
      PRNL;
    }

    // =======================================================================================================
    
    PVOIDFUN(valloc, SIZEARG, size_t each =  1, VERBOSEARG) {
      assert(size > 0);
      
      size = align_up(size, sizeof(PVOID)); // Align the pointer

      PRLINE;
      PRINTF("ALLOCATING MEMORY FROM THE FREE LIST @ 0x%lx = %ul!\n", PROOT, PROOT);
      PRLINE;
      PRINT("Bytes requested: ", size);
      
      PVOID       pvoid  {nullptr};
      palloc_node  pblock {nullptr};

      // try to find a big enough block to alloc
      FOR_EACH_BLOCK
        if (strategies::do_not_track_used::block_is_free(block) && (BSIZE(block) >= size))
        {
          pblock = &block;
          pvoid  = BSTART(block);

          PRINT("Selected block at", pblock);
          PRINT("With block start at", pvoid);
          PRHLINE;
          DESCRIBEP(pblock);
          PRLINE;

          goto found_a_block;
        }

      IFISNULL(pblock) {
        WARN("OUT OF MEMORY IN FREE LIST @ 0x%lx = %ul!!!!!!\n", PROOT, PROOT);        

        return nullptr;
      }
          
    found_a_block:
      
      // Check if we can we split the block:
      if ((BSIZEP(pblock) - size) >= MIN_ALLOC_SZ)
        split_blockp(pblock, size, verbose);
          
#ifndef NDEBUG
      else 
        DIE(
          "SUSPICIOUS ALLOC: not %zu - %zu = %zu >= %zu.\n",
          BSIZEP(pblock),
          size,
          (BSIZEP(pblock) - size),
          MIN_ALLOC_SZ);
#endif
      
      PRINT("Gave pointer to", pvoid);
      PRLINE;
      
      return pvoid;
    }

    // =======================================================================================================

    template <typename TT>
#ifdef RESEUNE_SINGLETON_ALLOCATOR
    static
#endif
    inline PVOID valloc(SIZEARG = 1, VERBOSEARG) {
      // Is this function dumb? Maybe we don't really need it?

      return valloc(size * sizeof(TT), 1, verbose);
    }
    
    // =======================================================================================================

    template <typename TT>
#ifdef RESEUNE_SINGLETON_ALLOCATOR
    static
#endif
    inline TT * alloc(SIZEARG = 1, VERBOSEARG) {
      return reinterpret_cast<TT *>(valloc(size * sizeof(TT), verbose));
    }
    
    // =======================================================================================================

    VOIDFUN(coalesce, VERBOSEARG) {
      PRLINE;
      PRINTF("COALESCING THE FREE LIST @ 0x%lx = %ul!\n", PROOT, PROOT);
      PRLINE;

      palloc_node plast_block {nullptr};

      FOR_EACH_BLOCK {
        IFISNOTNULL(plast_block)
          if (strategies::do_not_track_used::block_is_free(*plast_block)
              && strategies::do_not_track_used::block_is_free(block)) {
            if ((UINTPTR(BSTARTP(plast_block)) + BSIZEP(plast_block)) == UINTPTR(&block)) {
              SETBSIZEP(plast_block, BSIZEP(plast_block) + ALLOC_HEADER_SZ + BSIZE(block));

              PRINTF("Removing this block:.\n");
              DESCRIBE(block);

              block.remove();
            }
          }
        
        plast_block = &block;
      }

      PRHLINE;
      PRINTF("Done coalescing.\n");
      PRLINE;
      PRNL;
    }
  
    // =======================================================================================================
    
    VOIDFUN(release, ADDRARG, VERBOSEARG, bool defer_coalesce = false) {
      // WARNING: something bad will probably happen if you try to release an address that wasn't ever
      // by one of these allocators (such that the addr does not have an alloc_info in the memory loction
      // allocated directly to it's left).
      //
      // If you are using multiple allocator objects, you probably ~could~ get away with having one allocator
      // release a block that was allocated by a different allocator (effectively 'stealing' the block and
      // adding it to your own free list), but I haven't tried that yet (and whether doing so would ever be a
      // good idea seems kind of questionoable to me).
          
      palloc_node pnew_block {PALLOC_NODE(UINTPTR(addr) - ALLOC_HEADER_SZ)};

      PRLINE;
      PRINTF("RELEASING 0x%lx = %ul!\n", addr);
      PRLINE;
      PRINT("It's node is at", pnew_block);
      PRLINE;
      PRNL;
      
      ASSERTISNOTNULL(addr);
      
      strategies::do_not_track_used::release_block(*pnew_block, verbose);
      
      if (! defer_coalesce)
          coalesce(verbose);
    }

    // =======================================================================================================
  };

#ifdef RESEUNE_SINGLETON_ALLOCATOR
  template <typename T>
  allocator<T>::alloc_node allocator<T>::root {};
#endif
}

#include "undef_macros.hpp"  
#endif
