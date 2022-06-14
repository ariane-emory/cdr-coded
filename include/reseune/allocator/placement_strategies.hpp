#ifndef RESEUNE_ALLOCATOR_ALLOCATOR_PLACEMENT_STRATEGIES_HPP
#define RESEUNE_ALLOCATOR_ALLOCATOR_PLACEMENT_STRATEGIES_HPP

#include "macros.hpp"
#include "placement_strategies.hpp"
#include "reseune/doubly_linked/doubly_linked.hpp"

// =============================================================================================================
namespace reseune {

  // ===========================================================================================================
  namespace placement_strategies {

    // =========================================================================================================
    template <template <typename> typename container, typename alloc_node>
    struct pointer_order {};
    
    // =========================================================================================================
    template <typename alloc_node>
    struct pointer_order<doubly_linked, alloc_node> {
      
      // =======================================================================================================      
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

        // There shouldn't be no way for plast_block to be null if we got this far, no null checks needed.
              
        PRINTF("Placed block is at the end.\n");
              
        RCONS(new_block, plast_block); 
      }
    };
    
    // =========================================================================================================
  }
  // ===========================================================================================================
}
// =============================================================================================================
#include "undef_macros.hpp"

#endif
