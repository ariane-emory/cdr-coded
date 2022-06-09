#ifndef RESEUNE_MEMPOOL
#define RESEUNE_MEMPOOL

namespace reseune {
  template <typename T, size_t S>
  class mempool : public pool<T, S> {};

}


#endif
