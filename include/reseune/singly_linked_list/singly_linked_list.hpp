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
    

    // =================================================================================================================
    // Protected member fields
    // =================================================================================================================
    node_type * m_head;
    node_type * m_tail;

  protected:

    size_t      m_size;

    // =================================================================================================================
    // Protected member functions
    // =================================================================================================================
    node_type * seek(size_t index) {
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
    singly_linked_list(): m_head(nullptr), m_tail(nullptr), m_size(0) {}

    // =================================================================================================================
    // Destructor
    // =================================================================================================================
    virtual ~singly_linked_list() {
      clear();
    }

    // =================================================================================================================
    // Public member functions
    // =================================================================================================================
    void print() {
      printf("\nPrinting: Head: %zu, Tail: %zu\n", m_head, m_tail);
      for (size_t ix = 0; ix < size(); ix++)
        printf("Item #%zu @ %zu = '%i'.\n", ix, (*this)[ix], (*this)[ix]);
    }

    // =================================================================================================================
    void loop_print() {
      printf("\nPrinting: Head: %zu, Tail: %zu\n", m_head, m_tail);
      for (size_t ix = 0; ix < size(); ix++)
        printf("Item #%zu @ %zu = '%i'.\n", ix, (*this)[ix], (*this)[ix]);
    }

    // =====================================================================================================================
    void add(value_type const & element) {
      if (m_head == nullptr) {
        m_tail = m_head = new node_type(element);
        printf("Added head @ '%zu' pointing at '%zu'.\n", m_tail, m_tail->next);
      }
      else {
        m_tail->next = new node_type(element);
        printf("Added tail @ '%zu' pointing at '%zu'.\n", m_tail->next, m_tail->next->next);
        m_tail = m_tail->next;
      }

      m_size++;
    }

    // =================================================================================================================
    void insert(size_t index, value_type const & element) { // Inserts before index.
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
    value_type & operator[](size_t index) {
      return seek(index)->data;
    }

    // =================================================================================================================
    void clear() {
      while (m_head != nullptr)
        remove(0);
    }

    // =================================================================================================================
    void remove(size_t index) {
      // It is UB to remove from a list of length 0! Doing so will attempt to dereference a null pointer.
      
      printf("\nRemoving from: Head = %zu, tail = %zu: Removing %zu.\n", m_head, m_tail, index);
      
      node_type * target;

      if (index == 0) {
        target = m_head;
        printf("Removing target %zu.\n", target);
        m_head = target->next;
        if (m_head == nullptr)
          m_tail = m_head;
      }
      else {
        printf("This case happened!\n");
        node_type * pretarget = seek(index-1);
        target = pretarget->next;
        pretarget->next = target->next;
        if (pretarget->next == nullptr)
          m_tail = pretarget;
      }

      printf("About to destroy...\n");
      DT<value_type>::destroy(target->data);
      printf("Successfully destroyed.\n");
      
      delete target;
      m_size--;
    }

    // =================================================================================================================
    void set(size_t index, value_type const & element) {
      seek(index)->data = element;
    }

    // =================================================================================================================
    size_t size() const {
      return m_size;
    }

    // =================================================================================================================
  };
  // ===================================================================================================================
}
// =====================================================================================================================
#endif
