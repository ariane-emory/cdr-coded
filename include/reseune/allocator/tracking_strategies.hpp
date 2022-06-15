#ifndef RESEUNE_ALLOCATOR_ALLOCATOR_STRATEGIES_HPP
#define RESEUNE_ALLOCATOR_ALLOCATOR_STRATEGIES_HPP

#include "placement_strategies.hpp"
#include "macros.hpp" // include last
#include "reseune/alloc_info/alloc_info.hpp"
#include "reseune/alloc_info_with_unfree_flag/alloc_info_with_unfree_flag.hpp"
#include "reseune/doubly_linked/doubly_linked.hpp"
#include <tuple>

// =============================================================================================================
namespace reseune {

  // ===========================================================================================================
  namespace tracking_strategies {

    // =========================================================================================================
    template <
      template <typename> typename container,
      template <template <typename> typename> typename placement,
      typename alloc_info>
    struct standard {};
    
    // =========================================================================================================
    template <
      template <template <typename> typename> typename tplacement,
      typename alloc_info>
    struct standard<doubly_linked, tplacement, alloc_info> {
      template <typename t> using tcontainer = doubly_linked<t>;
      using alloc_node = tcontainer<alloc_info>;
      using placement = tplacement<tcontainer>;

      static inline void commit_block(alloc_node & block, VERBOSEARG) {
        (std::ignore = verbose);
        block.remove();
      }

      static inline void release_block(alloc_node & block, alloc_node & head, VERBOSEARG) {
        placement::place_block(block, head, verbose);
      }

      static inline bool block_is_free(alloc_node const & block, VERBOSEARG) {
        (std::ignore = verbose) = block;
        return true;
      }
    };

    // =========================================================================================================
    template <template <template <typename> typename> typename placement>
    struct standard<doubly_linked, placement, alloc_info_with_unfree_flag> {
      template <typename t> using container = doubly_linked<t>;
      using alloc_node = container<alloc_info_with_unfree_flag>;

      static inline void commit_block(alloc_node & block, VERBOSEARG) {
        (std::ignore = verbose);
        block.data.unfree = true;
      }

      static inline void release_block(alloc_node & block, alloc_node & head, VERBOSEARG) {
        (std::ignore = verbose) = head;        
        block.data.unfree = false;
      }

      static inline bool block_is_free(alloc_node const & block, VERBOSEARG) {
        (std::ignore = verbose);        
        return ! block.data.unfree;
      }
    };

    // =========================================================================================================
  };
// ===========================================================================================================
}
// =============================================================================================================
#endif
