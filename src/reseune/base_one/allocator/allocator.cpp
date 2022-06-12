#ifdef RESEUNE_SINGLETON_ALLOCATOR
#include "reseune/base_one/allocator/allocator.hpp"

reseune::base_one::allocator::alloc_node reseune::base_one::allocator::FREE_LIST {nullptr, nullptr};
#endif
