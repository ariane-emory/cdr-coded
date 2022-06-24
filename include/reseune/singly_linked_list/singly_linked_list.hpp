#ifndef RESEUNE_SINGLY_LINKED_LIST_H
#define RESEUNE_SINGLY_LINKED_LIST_H

#include <inttypes.h>

#include "../util/util.hpp"
#include "../delete_traits/delete_traits.hpp"

// =====================================================================================================================
namespace reseune {
  // ===================================================================================================================
  template <typename T, template<typename> class DT = delete_traits::owner>
  struct singly_linked_list {
    // =================================================================================================================
    // Types
    // =================================================================================================================
    using value_type = T;
    
  protected:

    // =================================================================================================================
    class node {
    public:
      value_type value;
      node* next;
      node(value_type const & _value) : value(_value), next(nullptr) {}
    };
    // =================================================================================================================

    // =================================================================================================================
    // Protected member fields
    // =================================================================================================================
    node * _head;
    node * _tail;
    size_t _size;

    // =================================================================================================================
    // Protected member functions
    // =================================================================================================================
    node * seek(size_t index) {
      if (index == 0)
        return _head;
      else if (index == _size - 1)
        return _tail;
      else {
        node * current = _head;

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
        _tail = new node(element);
        _head = _tail;
      }
      else {
        _tail->next = new node(element);
        _tail = _tail->next;
      }

      _size++;
    }

    // =================================================================================================================
    value_type & operator[](size_t index) {
      if (_size == 0)
        return seek(index)->value;
    }

    // =================================================================================================================
    void clear() {
      while (_head != nullptr)
        remove(0);
    }

    // =================================================================================================================
    void insert(size_t index, value_type const & element) override { // Inserts before index.
      if (index == 0) {
        node * tmp = _head;
        _head = new node(element);
        _head->next = tmp;
      }
      else {
        node * before = seek(index-1);
        node * after = before->next;

        before->next = new node(element);
        before->next->next = after;
      }

      _size++;
    }

    // =================================================================================================================
    void remove(size_t index) override {
      node * target;

      if (index == 0) {
        target = _head;
        _head = target->next;
        if (_head->next == nullptr)
          _tail = _head;
      }
      else {
        node * pretarget = seek(index-1);
        target = pretarget->next;
        pretarget->next = target->next;
        if (pretarget->next == nullptr)
          _tail = pretarget;
      }

      DT<value_type>::destroy(target->value);
      delete target;
      _size--;
    }

    // =================================================================================================================
    void set(size_t index, value_type const & element) override {
      seek(index)->value = element;
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
