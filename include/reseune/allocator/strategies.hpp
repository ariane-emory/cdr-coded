#ifndef RESEUNE_ALLOCATOR_ALLOCATOR_STRATEGIES_HPP
#define RESEUNE_ALLOCATOR_ALLOCATOR_STRATEGIES_HPP

#include "macros.hpp"

namespace reseune {
  // ===========================================================================================================
  
  template <typename T, template <typename> typename S>
  class allocator;

  // ===========================================================================================================

  namespace allocator_strategies {

    // =========================================================================================================

    template <typename alloc_node>
    struct doubly_linked_placement_stategy {
      // =======================================================================================================
    
    public:
    
      static inline void place_block(alloc_node & new_block, alloc_node & head, VERBOSEARG) {
        PRLINE;
        PRINT("Placing block", &new_block);

        alloc_node * plast_block {nullptr}; 
 
        FOR_EACH_BLOCK(head) {
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
    };
    
    // =========================================================================================================
    
    template <typename alloc_info>
       struct no_track {
       using allocator_type = allocator<alloc_info, no_track>;
       using alloc_node = typename allocator_type::alloc_node;

       static inline void commit_block(alloc_node & block, VERBOSEARG) {
         block.remove();
       }

       static inline void release_block(alloc_node & block, alloc_node & head, VERBOSEARG) {
         allocator_type::place_block(block, head, verbose);
       }

       static inline bool block_is_free(alloc_node const & block, VERBOSEARG) {
         return true;
       }
     };

     // =========================================================================================================

     template <typename alloc_info>
       struct track_by_marking {
      using alloc_node = typename allocator<alloc_info, track_by_marking>::alloc_node;

      static inline void commit_block(alloc_node & block, VERBOSEARG) {
        block.data.unfree = true;
      }

      static inline void release_block(alloc_node & block, alloc_node & head, VERBOSEARG) {
        block.data.unfree = false;
      }

      static inline bool block_is_free(alloc_node const & block, VERBOSEARG) {
        return ! block.data.unfree;
      }
    };

    // =========================================================================================================
  };
}

#include "undef_macros.hpp"

#endif
