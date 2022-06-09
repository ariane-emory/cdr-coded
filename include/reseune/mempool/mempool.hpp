#ifndef RESEUNE_MEMPOOL
#define RESEUNE_MEMPOOL

namespace reseune {
  template <typename T, size_t S>
  class mempool : public pool<T, S> {
    using value_type = T;
    
    struct ll_t {
      struct ll_head *next;
      struct ll_head *prev;
    };

    struct alloc_node_t {
      ll_t   node;
      size_t size;
      T *    block;
    };
  };
}

#endif
