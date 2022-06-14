#ifndef RESEUNE_ALLOCATOR_ALLOCATOR_STRATEGIES_HPP
#define RESEUNE_ALLOCATOR_ALLOCATOR_STRATEGIES_HPP

#include "placement_strategies.hpp"
#include "macros.hpp" // include last
#include "reseune/alloc_info/alloc_info.hpp"
#include "reseune/alloc_info_with_unfree_flag/alloc_info_with_unfree_flag.hpp"

// =============================================================================================================
namespace reseune {

  // ===========================================================================================================
  template <typename T>
  class allocator;

  // ===========================================================================================================
  namespace allocator_strategies {

    // =========================================================================================================
    template <typename alloc_node>
    struct ordinary {};
    
    // =========================================================================================================
    template <>
    struct ordinary<alloc_info> {
      using allocator_type = allocator<alloc_info>;
      using alloc_node = typename allocator_type::alloc_node;

      static inline void commit_block(alloc_node & block, VERBOSEARG) {
        block.remove();
      }

      static inline void release_block(alloc_node & block, alloc_node & head, VERBOSEARG); //  {
      //   allocator_type::placement::place_block(block, head, verbose);
      // }

      static inline bool block_is_free(alloc_node const & block, VERBOSEARG) {
        return true;
      }
      };

    // =========================================================================================================
    template <>
    struct ordinary<alloc_info_with_unfree_flag> {
      using alloc_node = typename allocator<alloc_info_with_unfree_flag>::alloc_node;

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
