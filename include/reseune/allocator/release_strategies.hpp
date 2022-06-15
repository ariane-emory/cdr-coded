#ifndef RESEUNE_ALLOCATOR_ALLOCATOR_RELEASE_STRATEGIES_HPP
#define RESEUNE_ALLOCATOR_ALLOCATOR_RELEASE_STRATEGIES_HPP

#include "placement_strategies.hpp"
#include "macros.hpp" // include last
#include "reseune/alloc_info/alloc_info.hpp"
#include "reseune/alloc_info_with_unfree_flag/alloc_info_with_unfree_flag.hpp"
#include "reseune/doubly_linked/doubly_linked.hpp"
#include <tuple>

// =============================================================================================================
namespace reseune {

  // ===========================================================================================================
  namespace release_strategies {

    // =========================================================================================================
    template <
      template <typename> typename container,
      template <template <typename> typename> typename placement>
    struct place_or_mark {};
    
    // =========================================================================================================
    template <
      template <template <typename> typename> typename tplacement>
    struct place_or_mark<doubly_linked, tplacement> {
      template <typename t> using container = doubly_linked<t>;

      template <typename alloc_info>
      static inline void release_block(
        container<alloc_info> & block,
        container<alloc_info> & head,
        VERBOSEARG) {
        tplacement<container>::place(block, head, verbose);
      }

      static inline void release_block(
        container<alloc_info_with_unfree_flag> & block,
        container<alloc_info_with_unfree_flag> & head,
        VERBOSEARG) {
        (std::ignore = verbose) = head;        
        block.data.unfree = false;
      }

      // =======================================================================================================
    };
    // =========================================================================================================
  };
// =============================================================================================================
}
// =============================================================================================================
#endif