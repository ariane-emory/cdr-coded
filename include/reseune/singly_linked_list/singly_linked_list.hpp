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
    void add(value_type const & element) {
      if (m_head == nullptr) {
        m_tail = new node_type(element);
        m_head = m_tail;
      }
      else {
        m_tail->next = new node_type(element);
        m_tail = m_tail->next;
      }

      m_size++;
    }

    // =================================================================================================================
    value_type & operator[](size_t index) {
      //if (m_size == 0)
      return seek(index)->data;
    }

    // =================================================================================================================
    size_t length() {
      return m_size;
    }

    // =================================================================================================================
    void clear() {
      while (m_head != nullptr)
        remove(0);
    }

    // =================================================================================================================
    void insert(size_t index, value_type const & element) { // Inserts before index.
      if (index == 0) {
        node_type * tmp = m_head;
        m_head = new node_type(element);
        m_head->next = tmp;
      }
      else {
        node_type * before = seek(index-1);
        node_type * after = before->next;

        before->next = new node_type(element);
        before->next->next = after;
      }

      m_size++;
    }

    // =================================================================================================================
    void remove(size_t index) {
      printf("\nHead = %zu, tail = %zu: Removing %zu.\n", m_head, m_tail, index);
      
      node_type * target;

      if (index == 0) {
        target = m_head;
        printf("Removing target %zu.\n", target);
        m_head = target->next;
        if (m_head->next == m_head)
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