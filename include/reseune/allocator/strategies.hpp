#ifndef RESEUNE_ALLOCATOR_ALLOCATOR_STRATEGIES_HPP
#define RESEUNE_ALLOCATOR_ALLOCATOR_STRATEGIES_HPP

#include "macros.hpp"

// =============================================================================================================
namespace reseune {

  // ===========================================================================================================
  template <typename T, template <typename> typename S>
  class allocator;

  // ===========================================================================================================
  namespace allocator_strategies {

    // =========================================================================================================
    template <typename alloc_info>
    struct no_track {
      using allocator_type = allocator<alloc_info, no_track>;
      using alloc_node = typename allocator_type::alloc_node;
      template <typename t> using container = allocator_type::container;

      static inline void commit_block(alloc_node & block, VERBOSEARG) {
        block.remove();
      }

      static inline void release_block(alloc_node & block, alloc_node & head, VERBOSEARG) {
        placement_strategies::pointer_order<container, alloc_node>::place_block(block, head, verbose);
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
  // ===========================================================================================================
}
// =============================================================================================================
#endif
