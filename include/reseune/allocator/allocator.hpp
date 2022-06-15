#ifndef RESEUNE_ALLOCATOR_HPP
#define RESEUNE_ALLOCATOR_HPP

#include <inttypes.h>
#include <stdint.h>
#include <cassert>

#include "reseune/util/util.hpp"
#include "reseune/alloc_info/alloc_info.hpp"
#include "reseune/doubly_linked/doubly_linked.hpp"

#include "placement_strategies.hpp"
#include "removal_strategies.hpp"
#include "commit_strategies.hpp"
#include "release_strategies.hpp"

#include "macros.hpp" // include this last!
// =============================================================================================================
namespace reseune {

  // ===========================================================================================================
  template <
    typename alloc_info,
    template <typename> typename tcontainer = doubly_linked,
    template <template <typename> typename> typename tinsert_in_order = placement_strategies::insert_in_pointer_order,
    template <template <typename> typename> typename tinsert_after = placement_strategies::insert_after,
    template <template <typename> typename> typename tremoval = removal_strategies::unlink,
    template <template <typename> typename> typename tcommit = commit_strategies,
    template <template <typename> typename, template <template <typename> typename> typename> typename trelease = release_strategies>
  class allocator {
 public:    
    using alloc_node      = tcontainer<alloc_info>;
    using insert_in_order = tinsert_in_order<tcontainer>;
    using insert_after    = tinsert_after<tcontainer>;
    using remove          = tremoval<tcontainer>;
    using commit_block    = tcommit<tcontainer>;
    using release_block   = trelease<tcontainer, tinsert_in_order>;
    
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

        insert_after::place(new_block, block, verbose);
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

      IFISNULL(PFREE_LIST_HEAD)
        insert_after::place(new_block, root, verbose);
      else
        insert_in_order::place(new_block, root, verbose);      
    
      PRLINE;
      PRNL;
    }
    
  private:

    // =======================================================================================================
    INLINE alloc_node * find_first_fit(SIZEARG, VERBOSEARG) {
      alloc_node * pblock {nullptr};
        
      // try to find a big enough block to alloc
      FOR_EACH_BLOCK(FREE_LIST_HEAD)
        if (commit_block::is_free(block, verbose) && (BSIZE(block) >= size))
        {
          pblock = &block;
          PVOID pvoid {BSTART(block)};

          PRINT("Selected block at", pblock);
          PRINT("With block start at", pvoid);
          PRHLINE;
          DESCRIBEP(pblock);
          PRLINE;

          return pblock;
        }

      return nullptr;
    }

  public:
      
    // =======================================================================================================    
    PVOIDFUN(valloc, SIZEARG, size_t each =  1, VERBOSEARG) {
#ifndef NDEBUG
      assert(size > 0);
#endif
      
      size = align_up(size * each, sizeof(PVOID)); // Align the pointer

      PRLINE;
      PRINTF("ALLOCATING MEMORY FROM THE FREE LIST @ 0x%016lx = %ul!\n", PROOT, PROOT);
      PRLINE;
      PRINT("Bytes requested: ", size);
      
      alloc_node * pblock {find_first_fit(size, verbose)};

      IFISNULL(pblock) {
        WARN("OUT OF MEMORY IN FREE LIST @ 0x%016lx = %ul!!!!!!\n", PROOT, PROOT);
        return nullptr;
      }

      alloc_node & block  {*pblock};
      PVOID       pvoid  {BSTART(block)};
      
      // Check if we can we split the block:
      if ((BSIZE(block) - size) >= MIN_ALLOC_SZ) {
        split_block(block, size, verbose);

        commit_block::commit_block(block, verbose);
        
        PRINT("Created new block at", &block);
        PRINT("With block start at", BSTART(block));
        PRHLINE;
        DESCRIBE(block);
        PRHLINE;
      }
#ifndef NDEBUG
      else {
        DIE(
          "SUSPICIOUS ALLOC: not %zu - %zu = %zu >= %zu.\n",
          BSIZE(block),
          size,
          (BSIZE(block) - size),
          MIN_ALLOC_SZ);
      }
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
          if (commit_block::is_free(*plast_block, true)
              && commit_block::is_free(block, true)) {

            alloc_node & last_block {*plast_block};
            
            if ((UINTPTR(BSTART(last_block)) + BSIZE(last_block)) == UINTPTR(&block)) {
              SETBSIZE(last_block, BSIZE(last_block) + ALLOC_HEADER_SZ + BSIZE(block));

              PRINTF("Removing this block:.\n");
              DESCRIBE(block);

              remove::remove(block, verbose);
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

      PRLINE;
      PRINTF("RELEASING 0x%016lx = %ul!\n", addr);
      PRLINE;

      ASSERTISNOTNULL(addr);
      ALLOC_NODEP_TO_REF(new_block, UINTPTR(addr) - ALLOC_HEADER_SZ);

      PRINT("It's node is at", &new_block);
      PRLINE;
      PRNL;
      
      release_block::release_block(new_block, FREE_LIST_HEAD, verbose);
      
      if (! defer_coalesce)
        coalesce(verbose);
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
  };

#ifdef RESEUNE_SINGLETON_ALLOCATOR
  template <
    typename ai,
    template <typename> typename tc,
    template <template <typename> typename> typename toi,
    template <template <typename> typename> typename tia,
    template <template <typename> typename> typename tr,
    template <template <typename> typename> typename tcom,
    template <template <typename> typename, template <template <typename> typename> typename> typename trel>
  allocator<ai, tc, toi, tia, tr, tcom, trel>::alloc_node allocator<ai, tc, toi, tia, tr, tcom, trel>::root {};
#endif
}

// =============================================================================================================
#include "undef_macros.hpp"  
#endif
