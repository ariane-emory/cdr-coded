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
    node_type * _head;
    node_type * _tail;
    size_t      _size;

    // =================================================================================================================
    // Protected member functions
    // =================================================================================================================
    node_type * seek(size_t index) {
      if (index == 0)
        return _head;
      else if (index == _size - 1)
        return _tail;
      else {
        node_type * current = _head;

        while (index-- > 0)
          current = current-> next;

        return current;
      }
    }

 public:

    // =================================================================================================================
    // Constructor
    // =================================================================================================================
    singly_linked_list(): _head(nullptr), _tail(nullptr), _size(0) {}

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
      if (_head == nullptr) {
        _tail = new node_type(element);
        _head = _tail;
      }
      else {
        _tail->next = new node_type(element);
        _tail = _tail->next;
      }

      _size++;
    }

    // =================================================================================================================
    value_type & operator[](size_t index) {
      //if (_size == 0)
      return seek(index)->data;
    }

    // =================================================================================================================
    size_t length() {
      return _size;
    }

    // =================================================================================================================
    void clear() {
      while (_head != nullptr)
        remove(0);
    }

    // =================================================================================================================
    void insert(size_t index, value_type const & element) { // Inserts before index.
      if (index == 0) {
        node_type * tmp = _head;
        _head = new node_type(element);
        _head->next = tmp;
      }
      else {
        node_type * before = seek(index-1);
        node_type * after = before->next;

        before->next = new node_type(element);
        before->next->next = after;
      }

      _size++;
    }

    // =================================================================================================================
    void remove(size_t index) {
      printf("/nHead = %zu, tail = %zu: Removing %zu.\n", _head, _tail, index);
      
      node_type * target;

      if (index == 0) {
        target = _head;
        printf("Removing target %zu.\n", target);
        _head = target->next;
        if (_head->next == _head)
          _tail = _head;
      }
      else {
        printf("This case happened!\n");
        node_type * pretarget = seek(index-1);
        target = pretarget->next;
        pretarget->next = target->next;
        if (pretarget->next == nullptr)
                 _tail = pretarget;
      }

      printf("About to destroy...\n");
      DT<value_type>::destroy(target->data);
      printf("Successfully destroyed.\n");
      
      delete target;
      _size--;
    }

    // =================================================================================================================
    void set(size_t index, value_type const & element) {
      seek(index)->data = element;
    }

    // =================================================================================================================
    size_t size() const {
      return _size;
    }

    // =================================================================================================================
  };
  // ===================================================================================================================
}
// =====================================================================================================================
#endif
