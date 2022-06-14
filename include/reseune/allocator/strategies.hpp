#ifndef RESEUNE_ALLOCATOR_ALLOCATOR_STRATEGIES_HPP
#define RESEUNE_ALLOCATOR_ALLOCATOR_STRATEGIES_HPP

#include "placement_strategies.hpp"
#include "macros.hpp" // include last
#include "reseune/alloc_info/alloc_info.hpp"
#include "reseune/alloc_info_with_unfree_flag/alloc_info_with_unfree_flag.hpp"
#include "reseune/doubly_linked/doubly_linked.hpp"

// =============================================================================================================
namespace reseune {

  // ===========================================================================================================
  namespace allocator_strategies {

    // =========================================================================================================
    template <template <typename> typename container, typename alloc_info, template <template <typename> typename, typename> typename placement>
    struct standard {};
    
    // =========================================================================================================
    template <typename alloc_info, template <template <typename> typename, typename> typename tplacement>
    struct standard<doubly_linked, alloc_info, tplacement> {
      template <typename t> using container = doubly_linked<t>;
      using alloc_node = container<alloc_info>;
      using placement = tplacement<container, alloc_node>;

      static inline void commit_block(alloc_node & block, VERBOSEARG) {
        block.remove();
      }

      static inline void release_block(alloc_node & block, alloc_node & head, VERBOSEARG) {
        placement::place_block(block, head, verbose);
      }

      static inline bool block_is_free(alloc_node const & block, VERBOSEARG) {
        return true;
      }
    };

    // =========================================================================================================
    // template <template <template <typename> typename, typename> typename placement, template <typename> typename container>
    template <template <template <typename> typename, typename> typename placement>
    struct standard<doubly_linked, alloc_info_with_unfree_flag, placement> {
      template <typename t> using container = doubly_linked<t>;
      using alloc_node = container<alloc_info_with_unfree_flag>;

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
// ===========================================================================================================
}
// =============================================================================================================
#endif
