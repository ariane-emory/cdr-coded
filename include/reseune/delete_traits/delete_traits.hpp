#ifndef RESEUNE_DELETE_TRAITS_H
#define RESEUNE_DELETE_TRAITS_H

#include <inttypes.h>
#include <type_traits>

// =====================================================================================================================
namespace reseune {
  // ===================================================================================================================
  class delete_traits {

  public:

    // =================================================================================================================
    template<typename T1>
    struct owner {
      static void destroy(T1 & x) {
        if constexpr (std::is_pointer<T1>::value) {
          printf("Destroy 1.\n");
          delete x;
        }
        else {
          printf("Destroy 2.\n");
        }
      }
    };
    
    // =================================================================================================================
    // template<typename T1>
    // struct owner<T1*> {
    //   static void destroy(T1* p) {  printf("Destroy 2.\n"); delete p; }
    // };

    // =================================================================================================================
    template<typename T1>
    struct non_owner {
      static void destroy(T1&) {  printf("Destroy 3.\n"); }
    };

    // =================================================================================================================
  };
  // ===================================================================================================================
}
// =====================================================================================================================
#endif
