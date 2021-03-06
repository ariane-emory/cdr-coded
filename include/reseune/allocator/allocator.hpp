#ifndef RESEUNE_ALLOCATOR_HPP
#define RESEUNE_ALLOCATOR_HPP

#include <inttypes.h>
#include <stdint.h>
#include <cassert>

#include "reseune/util/util.hpp"
#include "reseune/alloc_info/alloc_info.hpp"
#include "reseune/doubly_linked/doubly_linked.hpp"

#include "macros.hpp" // include last!

// =====================================================================================================================
namespace reseune {
  // ===================================================================================================================
  template <typename T>
  class allocator;

  // ===================================================================================================================
  struct strategies {
    struct no_track {
      template <typename alloc_node_t>
      static inline void commit_block(alloc_node_t & block, VERBOSEARG) {
        block.remove();
      }

      template <typename alloc_node_t>
      static inline void release_block(alloc_node_t & block, VERBOSEARG) {
        allocator<alloc_node_t>::place_block(block, verbose);
      }

      template <typename alloc_node_t>
      static inline bool block_is_free(alloc_node_t const & block, VERBOSEARG) {
        return true;
      }
    };

    struct track_by_marking {
      template <typename alloc_node_t>
      static inline void commit_block(alloc_node_t & block, VERBOSEARG) {
        block.data.unfree = true;
      }

      template <typename alloc_node_t>
      static inline void release_block(alloc_node_t & block, VERBOSEARG) {
        block.data.unfree = false;
      }

      template <typename alloc_node_t>
      static inline bool block_is_free(alloc_node_t const & block, VERBOSEARG) {
        return ! block.data.unfree;
      }
    };
  };

  // ===================================================================================================================
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
      
    using strategy = typename
#ifdef RESEUNE_USE_ALLOC_INFO_WITH_UNFREE_FLAG
      strategies::track_by_marking
#else
      strategies::no_track
#endif
      ;
    
  private:

    // =================================================================================================================
    VOIDFUN(split_block, alloc_node & block, SIZEARG, VERBOSEARG) {
      ALLOC_NODEP_TO_REF(new_block, UINTPTR(BSTART(block)) + size);
        
      SETBSIZE(new_block, BSIZE(block) - size - ALLOC_HEADER_SZ); // What happens when this is 0?
      SETBSIZE(block, size);
      RCONS(new_block, block);
      
      strategy::commit_block(block, verbose);

      PRINT("Created new block at", &new_block);
      PRINT("With block start at", BSTART(new_block));
      PRHLINE;
      DESCRIBE(new_block);
      PRHLINE;
    }
    
  public:
    
    // =================================================================================================================
    VOIDFUN(place_block, alloc_node & new_block, VERBOSEARG) {
      PRLINE;
      PRINT("Placing block", &new_block);
 
      IFISNULL(PFREE_LIST_HEAD) {
        PRINTF("Placing after FL.\n");
      
        RCONS(new_block, root);

        return;
      } 

      place_block_in_pointer_order(new_block, verbose);
    }
          
  private:
    
    // =================================================================================================================
    VOIDFUN(place_block_in_pointer_order, alloc_node & new_block, VERBOSEARG) {
      alloc_node * plast_block {nullptr}; 
 
      FOR_EACH_BLOCK {
#ifndef NDEBUG
        if (plast_block == &block) 
          DIE("last_blook == block, this is probably a logic error.\n");
#endif

        plast_block = &block; 
          
        PRINT("Compare with", uintptr(&block));
              
        if (plast_block <= &new_block) 
          continue;
                
        PRINTF("Placed block is before this block.\n"); 

        CONS(new_block, block); 

        return;
      } 

      // There shouldn't be any way for plast_block to be null if we got this far.
              
      PRINTF("Placed block is at the end.\n");
              
      RCONS(new_block, plast_block); 
    }
          
  public:

    // =================================================================================================================
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
            
      place_block(new_block, verbose);
                    
      PRLINE;
      PRNL;
    }

    // =================================================================================================================
    VOIDFUN(describe_free_list) {
      const bool verbose {true};

      PRLINE;
      PRINTF("PRINTING THE FREE LIST @ 0x%016lx = %ul!\n", PROOT, PROOT);
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

  private:

    // =================================================================================================================
    INLINE alloc_node * find_first_fit(SIZEARG, VERBOSEARG) {
      alloc_node * pblock {nullptr};
        
      // try to find a big enough block to alloc
      FOR_EACH_BLOCK
        if (strategy::block_is_free(block, verbose) && (BSIZE(block) >= size))
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
    
    // =================================================================================================================
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
      PVOID        pvoid  {BSTART(block)};
      
      // Check if we can we split the block:
      if ((BSIZE(block) - size) >= MIN_ALLOC_SZ) {
        split_block(block, size, verbose);

        strategy::commit_block(block, verbose);
        
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

    // ================================================================================================================
    template <typename TT>
#ifdef RESEUNE_SINGLETON_ALLOCATOR
    static
#endif
    inline PVOID valloc(SIZEARG = 1, VERBOSEARG) {
      // Is this function dumb? Maybe we don't really need it?

      return valloc(size * sizeof(TT), 1, verbose);
    }
    
    // =================================================================================================================
    template <typename TT>
#ifdef RESEUNE_SINGLETON_ALLOCATOR
    static
#endif
    inline TT * alloc(SIZEARG = 1, VERBOSEARG) {
      return reinterpret_cast<TT *>(valloc(size * sizeof(TT), verbose));
    }
    
    // =================================================================================================================
    VOIDFUN(coalesce, VERBOSEARG) {
      PRLINE;
      PRINTF("COALESCING THE FREE LIST @ 0x%016lx = %ul!\n", PROOT, PROOT);
      PRLINE;

      alloc_node * plast_block {nullptr};

      FOR_EACH_BLOCK {
        IFISNOTNULL(plast_block)
          if (strategy::block_is_free(*plast_block, verbose)
              && strategy::block_is_free(block, verbose)) {

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
  
    // =================================================================================================================
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
      
      strategy::release_block(new_block, verbose);
      
      if (! defer_coalesce)
        coalesce(verbose);
    }

    // =================================================================================================================
  };

#ifdef RESEUNE_SINGLETON_ALLOCATOR
  template <typename T>
  allocator<T>::alloc_node allocator<T>::root {};
#endif
}
// =====================================================================================================================
// Don't leak the macros!
// =====================================================================================================================
#include "undef_macros.hpp" 
// =====================================================================================================================
#endif
