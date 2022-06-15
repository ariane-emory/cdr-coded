#ifndef RESEUNE_ALLOCATOR_ALLOCATOR_COMMIT_STRATEGIES_HPP
#define RESEUNE_ALLOCATOR_ALLOCATOR_COMMIT_STRATEGIES_HPP

#include "placement_strategies.hpp"
#include "macros.hpp" // include last
#include "reseune/alloc_info/alloc_info.hpp"
#include "reseune/alloc_info_with_unfree_flag/alloc_info_with_unfree_flag.hpp"
#include "reseune/doubly_linked/doubly_linked.hpp"
#include <tuple>

// =============================================================================================================
namespace reseune {

  // =========================================================================================================
  template <template <typename> typename container>
  struct commit_strategies {};
    
  // =========================================================================================================
  template <>
  struct commit_strategies<doubly_linked> {
    template <typename t> using container = doubly_linked<t>;

    template <typename alloc_info>
    static inline void commit_block(
      container<alloc_info> & block,
      VERBOSEARG) {
      (std::ignore = verbose);
      block.remove();
    }
            
    template <typename alloc_info>
    static inline bool is_free(
      container<alloc_info> const & block,
      VERBOSEARG) {
      (std::ignore = verbose) = block;
      return true;
    }

    static inline void commit_block(
      container<alloc_info_with_unfree_flag> & block,
      VERBOSEARG) {
      (std::ignore = verbose);
      block.data.unfree = true;
    }

    static inline bool is_free(
      container<alloc_info_with_unfree_flag> const & block,
      VERBOSEARG) {
      (std::ignore = verbose);        
      return ! block.data.unfree;
    }
  };

  // =========================================================================================================
};
// =============================================================================================================
#endif
