#ifndef RESEUNE_ALLOCATOR_ALLOCATOR_REMOVAL_STRATEGIES_HPP
#define RESEUNE_ALLOCATOR_ALLOCATOR_REMOVAL_STRATEGIES_HPP

#include "reseune/doubly_linked/doubly_linked.hpp"
#include "reseune/util/util.hpp"
#include "removal_strategies.hpp"

#include "macros.hpp" // include last

// =============================================================================================================
namespace reseune {

  // ===========================================================================================================
  namespace removal_strategies {

    // =========================================================================================================
    template <template <typename> typename container>
    struct unlink_or_mark {};
    
    // =========================================================================================================
    template <>
    struct unlink_or_mark<doubly_linked> {
      
      // =======================================================================================================
      template <typename item_t>
      static inline void remove(doubly_linked<item_t> & item, VERBOSEARG) {
        PRINTF("Removing item @ 0x%lx = %ul.\n", &item);

        item.remove();
      }
    };
    
    // =========================================================================================================
  }
  // ===========================================================================================================
}
// =============================================================================================================

#include "undef_macros.hpp" 
#endif
