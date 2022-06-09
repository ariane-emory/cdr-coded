#ifndef RESEUNE_MEMPOOL
#define RESEUNE_MEMPOOL

namespace reseune {
  template <typename T, size_t S>
  class mempool : public pool<T, S> {
    struct ll_t {
      struct ll_head *next;
      struct ll_head *prev;
    };

    struct alloc_node_t {
      ll_t node;
      size_t size;
      char * block;
    };
  };
}

#endif
