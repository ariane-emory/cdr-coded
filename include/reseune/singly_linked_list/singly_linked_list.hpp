#ifndef RESEUNE_SINGLY_LINKED_LIST_H
#define RESEUNE_SINGLY_LINKED_LIST_H

#include <inttypes.h>

#include "../util/util.hpp"
#include "../delete_traits/delete_traits.hpp"
#include "../singly_linked/singly_linked.hpp"

// =====================================================================================================================
namespace reseune {
  // ===================================================================================================================
  template <typename T, template<typename> class DT = delete_traits::owner>
  struct singly_linked_list {
    // =================================================================================================================
    // Types
    // =================================================================================================================
    using value_type = T;
    using node_type  = singly_linked<T>;
    
  protected:

    // =================================================================================================================
    // Protected member fields
    // =================================================================================================================
    node_type * m_head;
    node_type * m_tail;
    size_t      m_size;

    // =================================================================================================================
    // Protected member functions
    // =================================================================================================================
    node_type * seek(size_t index) {
      // It UB to seek an index >= size().
      
      if (index == 0)
        return m_head;
      else if (index == m_size - 1)
        return m_tail;
      else {
        node_type * current = m_head;

        while (index-- > 0)
          current = current-> next;

        return current;
      }
    }

 public:

    // =================================================================================================================
    // Constructor
    // =================================================================================================================
    constexpr singly_linked_list(): m_head(nullptr), m_tail(nullptr), m_size(0) {}

    // =================================================================================================================
    // Destructor
    // =================================================================================================================
    constexpr virtual ~singly_linked_list() {
      clear();
    }

    // =================================================================================================================
    // Public member functions
    // =================================================================================================================
    constexpr void add(value_type const & element) {
      if (m_head == nullptr) {
        m_tail = m_head = new node_type(element);
        printf("Added head @ '%zu' -> '%zu': '%i'.\n", m_tail, m_tail->next, m_tail->data);
      }
      else {
        m_tail->next = new node_type(element);
        printf("Added tail @ '%zu' -> '%zu': '%i'.\n", m_tail->next, m_tail->next->next, m_tail->next->data);
        m_tail = m_tail->next;
      }

      m_size++;
    }

    // =================================================================================================================
    constexpr void insert(size_t index, value_type const & element) {
      // Inserts before index.
      if (index == 0) {
        printf("Left case.\n");
        
        node_type * tmp = m_head;
        m_head = new node_type(element);
        m_head->next = tmp;
      }
      else {
        printf("Right case, element = '%i'.\n", element);
        
        node_type * before = seek(index-1);
        node_type * after  = before->next;

        before->next = new node_type(element);
        printf("New node's value is '%i'.\n", before->next->data);
        before->next->next = after;
      }

      m_size++;
    }

    // =================================================================================================================
    constexpr value_type & operator[](size_t index) {
      return seek(index)->data;
    }

    // =================================================================================================================
    constexpr void clear() {
      while (m_head != nullptr)
        remove(0);
    }

    // =================================================================================================================
    constexpr void remove(size_t index) {
      // It is UB to remove from a list of length 0! Doing so will attempt to dereference a null pointer.
      
      // printf("\nRemoving from: Head @ %zu, tail @xs %zu: Removing index %zu.\n", m_head, m_tail, index);
      
      node_type * target;

      if (index == 0) {
        target = m_head;
        // printf("Removing target %zu.\n", target);
        m_head = target->next;
        if (m_head == nullptr)
          m_tail = m_head;
      }
      else {
        // printf("This case happened!\n");
        node_type * pretarget = seek(index-1);
        target = pretarget->next;
        pretarget->next = target->next;
        if (pretarget->next == nullptr)
          m_tail = pretarget;
      }

      // printf("About to destroy...\n");
      DT<value_type>::destroy(target->data);
      // printf("Successfully destroyed.\n");
      
      delete target;
      m_size--;
    }

    // =================================================================================================================
    constexpr void set(size_t index, value_type const & element) {
      // I am shocked that this does not leak memory when value_type is a pointer? It looks like it should, but
      // seems not to?
      seek(index)->data = element;
    }

    // =================================================================================================================
    constexpr size_t size() const {
      return m_size;
    }

    // =====================================================================================================================
    // Some stupid debug functions and tests.
    // =====================================================================================================================
    constexpr void print() {
      printf("Printing: Head @ %zu, Tail @ %zu, Size = %zu.\n", m_head, m_tail, size());
      node_type * cursor = m_head;
      size_t ix = 0;
      
      while (cursor != nullptr) {
        printf("Item #%zu    @ %zu -> %zu: '%i'.\n", ++ix, cursor, cursor->next, cursor->data);
        cursor = cursor->next;
      }
    }

    // =====================================================================================================================
    static constexpr void run_tests() {
      run_ptr_tests();
      run_non_ptr_tests();
    }
    
    // =====================================================================================================================
    static constexpr void run_non_ptr_tests() {
      singly_linked_list<uintptr_t> sll;

      printf("\nTesting 'add'.\n");
      sll.add(8);
      sll.add(9);
      sll.add(3);
      printf("\n");
      sll.print();
      printf("\n");

      assert(3 == sll.size());
      assert(8 == sll[0]);
      assert(9 == sll[1]);
      assert(3 == sll[2]);
  
      printf("Testing 'set'.\n");
      sll.set(1, 2);
      printf("\n");
      sll.print();
      printf("\n");

      assert(3 == sll.size());
      assert(8 == sll[0]);
      assert(2 == sll[1]);
      assert(3 == sll[2]);  

      printf("Testing 'set' again.\n");
      sll.set(0, 1);
      printf("\n");
      sll.print();
      printf("\n");

      assert(3 == sll.size());
      assert(1 == sll[0]);
      assert(2 == sll[1]);
      assert(3 == sll[2]);  

      printf("Testing 'insert'.\n");
      sll.insert(2, 9);
      printf("\n");
      sll.print();
  
      assert(4 == sll.size());
      assert(1 == sll[0]);
      assert(2 == sll[1]);
      assert(9 == sll[2]);
      assert(3 == sll[3]);

      printf("\n");
      printf("Testing 'remove'.\n");
      sll.remove(2);
      printf("\n");
      sll.print();

      assert(3 == sll.size());
      assert(1 == sll[0]);
      assert(2 == sll[1]);
      assert(3 == sll[2]);

      printf("\n");
      printf("Testing 'clear'.\n");
      sll.clear();
      printf("\n");
  
      sll.print();
      assert(sll.size() == 0);
  
      printf("Passed tests!\n", sll.size());
    }

    // =====================================================================================================================
    static constexpr void run_ptr_tests() {
      singly_linked_list<uintptr_t *> sll;

      printf("\nTesting 'add'.\n");
      sll.add(new uintptr_t(8));
      sll.add(new uintptr_t(9));
      sll.add(new uintptr_t(3));
      printf("\n");
      sll.print();
      printf("\n");

      assert(3 == sll.size());
      assert(8 == *sll[0]);
      assert(9 == *sll[1]);
      assert(3 == *sll[2]);
  
      printf("Testing 'set'.\n");
      sll.set(1, new uintptr_t(2));
      printf("\n");
      sll.print();
      printf("\n");

      assert(3 == sll.size());
      assert(8 == *sll[0]);
      assert(2 == *sll[1]);
      assert(3 == *sll[2]);  

      printf("Testing 'set' again.\n");
      sll.set(0, new uintptr_t(1));
      printf("\n");
      sll.print();
      printf("\n");

      assert(3 == sll.size());
      assert(1 == *sll[0]);
      assert(2 == *sll[1]);
      assert(3 == *sll[2]);  

      printf("Testing 'insert'.\n");
      sll.insert(2, new uintptr_t(9));
      printf("\n");
      sll.print();
  
      assert(4 == sll.size());
      assert(1 == *sll[0]);
      assert(2 == *sll[1]);
      assert(9 == *sll[2]);
      assert(3 == *sll[3]);

      printf("\n");
      printf("Testing 'remove'.\n");
      sll.remove(2);
      printf("\n");
      sll.print();

      assert(3 == sll.size());
      assert(1 == *sll[0]);
      assert(2 == *sll[1]);
      assert(3 == *sll[2]);

      printf("\n");
      printf("Testing 'clear'.\n");
      sll.clear();
      printf("\n");
  
      sll.print();
      assert(sll.size() == 0);
  
      printf("Passed tests!\n", sll.size());
    }

    // =================================================================================================================
  };
  // ===================================================================================================================
}
// =====================================================================================================================
#endif
