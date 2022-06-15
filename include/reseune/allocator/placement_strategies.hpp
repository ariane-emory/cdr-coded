#ifndef RESEUNE_ALLOCATOR_ALLOCATOR_PLACEMENT_STRATEGIES_HPP
#define RESEUNE_ALLOCATOR_ALLOCATOR_PLACEMENT_STRATEGIES_HPP

#include "reseune/alloc_info/alloc_info.hpp"
#include "reseune/doubly_linked/doubly_linked.hpp"
#include "reseune/util/util.hpp"
#include "placement_strategies.hpp"

#include "macros.hpp" // include last

// =============================================================================================================
namespace reseune {

  // ===========================================================================================================
  namespace placement_strategies {

    // =========================================================================================================
    template <template <typename> typename container>
    struct insert_in_pointer_order {};
    
    // =========================================================================================================
    template <>
    struct insert_in_pointer_order<doubly_linked> {
      
      // =======================================================================================================
      template <typename alloc_node>
      static inline void place(alloc_node & new_item, alloc_node & head, VERBOSEARG) {
        PRLINE;
        PRINT("Placing block", &new_item);

        alloc_node * plast_item {nullptr}; 
        
        for (auto & item : head) {
#ifndef NDEBUG
          if (plast_item == &item) 
            DIE("last_item == item, this is probably a logic error.\n");
#endif

          plast_item = &item; 

          PRINT("Compare with", uintptr(&item));
              
          if (plast_item <= &new_item) 
            continue;
                
          PRINTF("Item being placed is before this item.\n"); 

          CONS(new_item, item); 

          return;
        } 

        // There shouldn't be no way for plast_item to be null if we got this far, no null checks needed.
              
        PRINTF("Placed block is at the end.\n");
              
        RCONS(new_item, plast_item); 
      }
    };
    
    // =========================================================================================================
    template <template <typename> typename container>
    struct after {};
    
    // =========================================================================================================
    template <>
    struct after<doubly_linked> {      
      // =======================================================================================================
      template <typename alloc_node>
      static inline void place(
        doubly_linked<alloc_node> & new_item,
        doubly_linked<alloc_node> & item,
        VERBOSEARG) {
        PRINTF("Placing after given item.\n");

        new_item.insert_after(item);
      }
    };
    
    // =========================================================================================================
    template <template <typename> typename container>
    struct before {};
    
    // =========================================================================================================
    template <>
    struct before<doubly_linked> {      
      // =======================================================================================================
      template <typename alloc_node>
      static inline void place(
        doubly_linked<alloc_node> & new_item,
        doubly_linked<alloc_node> & item,
        VERBOSEARG) {
        PRINTF("Placing before given item.\n");

        new_item.insert_before(item);
      }
    };
    
    // =========================================================================================================
  }
  // ===========================================================================================================
}
// =============================================================================================================
#include "undef_macros.hpp"

#endif
