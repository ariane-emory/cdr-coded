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
        static alloc_node FREE_LIST {nullptr, nullptr};
#else
        alloc_node FREE_LIST;

        // =======================================================================================================

        constexpr allocator() : FREE_LIST {nullptr, nullptr } {}
#endif
      
        // =======================================================================================================

#ifdef RESEUNE_SINGLETON_ALLOCATOR
        namespace {
#else
        private:
#endif

          VOIDFUN(SPLIT_BLOCKP, palloc_node pblock, SIZEARG, VERBOSEARG) {
            alloc_node & block      {*pblock};
            alloc_node & new_block  {*PALLOC_NODE((UINTPTR(BSTART(block)) + size))};
        
            SETBSIZE(new_block, BSIZE(block) - size - ALLOC_HEADER_SZ); // What happens when this is 0?
            SETBSIZE(block, size);
            RCONS(new_block, block);
            REMOVE(block);

            PRINT("Created new block at", &new_block);
            PRINT("With block start at", BSTART(new_block));
            PRHLINE;
            DESCRIBE(new_block);
            PRHLINE;
          }
          
          VOIDFUN(PLACE_BLOCKP, palloc_node pnew_block, VERBOSEARG) {
            PRLINE;
            PRINT("Placing block", pnew_block);
 
            IFISNULL(PFREE_LIST_HEAD) {
              PRINTF("Placing after FL.\n");
      
              RCONSP(pnew_block, FREE_LIST); 
            } 
            else { 
              PRINTF("Placing somewhere else.\n"); 
 
              palloc_node last_block {nullptr}; 
 
              FOR_EACH_BLOCK { 
                last_block = &block; 

                PRINTF("Compare with", &block);
              
                if (&block <= pnew_block) { 
                  PRINTF("PLACED BLOCK IS AFTER THIS BLOCK.\n");
                } 
                else { 
                  PRINTF("PLACED BLOCK IS BEFORE THIS BLOCK.\n"); 

                  CONSP(pnew_block, block); 

                  return;
                } 
              } 

              // There shouldn't be any way for last_block to be nil if we got this far.
              RCONSP(pnew_block, last_block); 
            } 
          }
          
#ifdef RESEUNE_SINGLETON_ALLOCATOR
        };
#else
      public:
#endif
        // =======================================================================================================
        
        VOIDFUN(add_memory, ADDRARG, SIZEARG, VERBOSEARG) {
          PRLINE;
          PRINTF("ADDING NEW MEMORY TO THE FREE LIST @ 0x%lx = %ul!\n", PFREE_LIST, PFREE_LIST);
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
            
          PLACE_BLOCKP(pnew_block);
                    
          PRLINE;
          PRNL;
        }

        // =======================================================================================================

        VOIDFUN(initialize, VERBOSEARG) {
#ifdef RESEUNE_BASE_ONE_MEMORY
          add_memory(MEMORY, MEMORY_BYTES, verbose);
#else
          DIE("initialize must not be called when -DRESEUNE_NO_BASE_ONE_MEMORY.");
#endif
        }
    
        // =======================================================================================================
    
        VOIDFUN(describe_free_list) {
          const bool verbose {true};

          PRLINE;
          PRINTF("PRINTING THE FREE LIST @ 0x%lx = %ul!\n", PFREE_LIST, PFREE_LIST);
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

              goto found_a_block;
            }

          IFISNULL(pblock) {
            WARN("OUT OF MEMORY IN FREE LIST @ 0x%lx = %ul!!!!!!\n", PFREE_LIST, PFREE_LIST);        

            return nullptr;
          }
          
        found_a_block:
      
          // Check if we can we split the block:
          if ((BSIZEP(pblock) - size) >= MIN_ALLOC_SZ)
            SPLIT_BLOCKP(pblock, size, verbose);
#ifndef NDEBUG
          else 
            DIE(
              "SUSPICIOUS ALLOC: not %zu - %zu = %zu >= %zu.\n",
              BSIZEP(pblock),
              size,
              (BSIZEP(pblock) - size),
              MIN_ALLOC_SZ);
#endif
      
          PRINT("Gave pointer to", UINTPTR(pvoid));
          PRLINE;
      
          return pvoid;
        }

        // =======================================================================================================

        template <typename T>
#ifdef RESEUNE_SINGLETON_ALLOCATOR
        static
#endif
        inline PVOID valloc(SIZEARG = 1, VERBOSEARG) {
          // Is this function dumb? Maybe we don't really need it?

          return valloc(size * sizeof(T), verbose);
        }
    
        // =======================================================================================================

        template <typename T>
#ifdef RESEUNE_SINGLETON_ALLOCATOR
        static
#endif
        inline T * alloc(SIZEARG = 1, VERBOSEARG) {
          return reinterpret_cast<T *>(valloc(size * sizeof(T), verbose));
        }
    
        // =======================================================================================================

        VOIDFUN(coalesce, VERBOSEARG) {
          PRLINE;
          PRINTF("COALESCEMMENTING THE FREE LIST @ 0x%lx = %ul!\n", PFREE_LIST, PFREE_LIST);
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
          PRINTF("Done coalesceing.\n");
          PRLINE;
          PRNL;
        }
  
        // =======================================================================================================
    
        VOIDFUN(release, ADDRARG, bool defer_coalesce = false, VERBOSEARG) {
          // WARNING: something bad will probably happen if you try to release an address that wasn't ever allocated
          // by one of these allocators (such that addr does not have an alloc_info in the memory loction directly
          // to it's left).
          //
          // If you are using multiple allocator objects, you probably ~could~ get away with having one allocator
          // release a block that was allocated by a different allocator (effectively 'stealing' the block and
          // adding it to your own free list, but I haven't tested this yet (and whether doing so would ever be a
          // good idea seems kind of questionoable to me).
          
          palloc_node pnew_block {PALLOC_NODE(UINTPTR(addr) - ALLOC_HEADER_SZ)};

          PRLINE;
          PRINTF("RELEASING 0x%lx = %ul!\n", UINTPTR(addr));
          PRLINE;
          PRINT("It's node is at", pnew_block);
          PRLINE;
          PRNL;
      
          ASSERTISNOTNULL(addr);
            
          PLACE_BLOCKP(pnew_block);
          
          if (! defer_coalesce)
            coalesce(verbose);
        }

        // =======================================================================================================
      };
    }
  }

#include "undef_macros.hpp"  
#endif
