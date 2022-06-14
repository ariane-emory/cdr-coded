#ifndef RESEUNE_ALLOCATOR_HPP
#define RESEUNE_ALLOCATOR_HPP

#include <inttypes.h>
#include <stdint.h>
#include <cassert>

#include "reseune/util/util.hpp"
#include "reseune/alloc_info/alloc_info.hpp"
#include "reseune/doubly_linked/doubly_linked.hpp"

#include "placement_strategies.hpp"
#include "tracking_strategies.hpp"
#include "macros.hpp" // include this last!

// =============================================================================================================
namespace reseune {

  // ===========================================================================================================
  template <
    typename alloc_info,
    template <typename> typename tcontainer = doubly_linked,
    template <template <typename> typename, typename, template <template <typename> typename, typename> typename> typename ttracking = tracking_strategies::standard,
    template <template <typename> typename, typename> typename tplacement = placement_strategies::pointer_order>
  class allocator {
  public:    
    using alloc_node = tcontainer<alloc_info>;
    using strategy   = ttracking<tcontainer, alloc_info, tplacement>; // S<alloc_info>;
    using placement  = tplacement<tcontainer, alloc_node>;
    
  private:
#ifdef RESEUNE_SINGLETON_ALLOCATOR
    static alloc_node root;
  private:
    allocator() {};
#else
    alloc_node root;
#endif
    
    // =======================================================================================================    
      VOIDFUN(split_block, alloc_node & block, SIZEARG, VERBOSEARG) {
        ALLOC_NODEP_TO_REF(new_block, UINTPTR(BSTART(block)) + size);
        
        SETBSIZE(new_block, BSIZE(block) - size - ALLOC_HEADER_SZ); // What happens when this is 0?
        SETBSIZE(block, size);
        RCONS(new_block, block);
      
        strategy::commit_block(block);

        PRINT("Created new block at", &new_block);
        PRINT("With block start at", BSTART(new_block));
        PRHLINE;
        DESCRIBE(new_block);
        PRHLINE;
      }
        
  public:

    // =======================================================================================================
    VOIDFUN(add_memory, ADDRARG, SIZEARG, VERBOSEARG) {
      PRLINE;
      PRINTF("ADDING NEW MEMORY TO THE FREE LIST @ 0x%016lx = %ul!\n", PROOT, PROOT);
      PRLINE;
      PRINT("Given memory at", addr);
      PRINT("Given bytes", size);
      
      ASSERTISNOTNULL(addr);

      // align the start addr of our pnew_block to the next pointer aligned addr
      ALLOC_NODEP_TO_REF(new_block, align_up(UINTPTR(addr), sizeof(PVOID)));
          
      PRINT("Align pnew_block to", &new_block);
          
      // calculate actual size - overhead
      SETBSIZE(
        new_block, 
        UINTPTR(addr)
        + size
        - UINTPTR(&new_block)
        - ALLOC_HEADER_SZ);

      IFISNULL(PFREE_LIST_HEAD) {
        PRINTF("Placing after phead.\n");
      
        RCONS(new_block, root);
        return;
      } 

      placement::place_block(new_block, FREE_LIST_HEAD, verbose);      
    
      PRLINE;
      PRNL;
    }
    
    // =======================================================================================================
    VOIDFUN(describe_free_list) {
      const bool verbose {true};

      PRLINE;
      PRINTF("PRINTING THE FREE LIST @ 0x%016lx = %ul!\n", PROOT, PROOT);
      PRLINE;

      ASSERTISNOTNULL(PFREE_LIST_HEAD);

      size_t ix {0};
      
      FOR_EACH_BLOCK(FREE_LIST_HEAD) {
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
#ifndef NDEBUG
      assert(size > 0);
#endif
      
      size = align_up(size, sizeof(PVOID)); // Align the pointer

      PRLINE;
      PRINTF("ALLOCATING MEMORY FROM THE FREE LIST @ 0x%016lx = %ul!\n", PROOT, PROOT);
      PRLINE;
      PRINT("Bytes requested: ", size);
      
      PVOID        pvoid  {nullptr};
      alloc_node * pblock {nullptr};

      // try to find a big enough block to alloc
      FOR_EACH_BLOCK(FREE_LIST_HEAD)
        if (strategy::block_is_free(block) && (BSIZE(block) >= size))
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
        WARN("OUT OF MEMORY IN FREE LIST @ 0x%016lx = %ul!!!!!!\n", PROOT, PROOT);        

        return nullptr;
      }
      
    found_a_block:
      alloc_node & block {*pblock};
      
      // Check if we can we split the block:
      if ((BSIZE(block) - size) >= MIN_ALLOC_SZ)
        split_block(block, size, verbose);
          
#ifndef NDEBUG
      else 
        DIE(
          "SUSPICIOUS ALLOC: not %zu - %zu = %zu >= %zu.\n",
          BSIZE(block),
          size,
          (BSIZE(block) - size),
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
      PRINTF("COALESCING THE FREE LIST @ 0x%016lx = %ul!\n", PROOT, PROOT);
      PRLINE;

      alloc_node * plast_block {nullptr};

      FOR_EACH_BLOCK(FREE_LIST_HEAD) {
        IFISNOTNULL(plast_block)
          if (strategy::block_is_free(*plast_block) && strategy::block_is_free(block)) {

            alloc_node & last_block {(*reinterpret_cast<alloc_node *>(plast_block))};
            
            if ((UINTPTR(BSTART(last_block)) + BSIZE(last_block)) == UINTPTR(&block)) {
              SETBSIZE((*plast_block), BSIZE(last_block) + ALLOC_HEADER_SZ + BSIZE(block));

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
          
      ALLOC_NODEP_TO_REF(new_block, UINTPTR(addr) - ALLOC_HEADER_SZ);
      
      PRLINE;
      PRINTF("RELEASING 0x%016lx = %ul!\n", addr);
      PRLINE;
      PRINT("It's node is at", &new_block);
      PRLINE;
      PRNL;
      
      ASSERTISNOTNULL(addr);
      
      strategy::release_block(new_block, FREE_LIST_HEAD, verbose);
      
      if (! defer_coalesce)
        coalesce(verbose);
    }

    // =======================================================================================================
  };

#ifdef RESEUNE_SINGLETON_ALLOCATOR
  template <typename T, template <typename> typename S>
  allocator<T, S>::alloc_node allocator<T, S>::root {};
#endif
}

#include "undef_macros.hpp"  
#endif
