#ifndef __CONCURRENT_LIST_H__
#define __CONCURRENT_LIST_H__

#include <cinttypes>
#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <utility>
#ifdef __USE_GNU_CAS
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#elif defined __USE_PIN_CAS
#include "atomic/ops.hpp"
#endif

/**
 * A wrapper for different CAS implemention, which only support integral scalar
 * or pointer types that are 1, 2, 4 or 8 bytes in length
 */
template <typename V>
inline V compare_and_swap(V* location, V oldVal, V newVal) {
#ifdef __USE_GNU_CAS
    return __sync_val_compare_and_swap(location, oldVal, newVal);
#elif defined __USE_PIN_CAS
    return ATOMIC::OPS::CompareAndSwap(location, oldVal, newVal);
#else
    if (*location == oldVal) {
        *location = newVal;
    } else {
        oldVal = *location;
    }
    return oldVal;
#endif
}

template <typename V>
class ConcurrentList {
   public:
    typedef V Value;

   private:
    struct Element {
        Value value;
        Element* next;
        Element(const Value& value, Element* next) : value(value), next(next) {}
        virtual ~Element() {}
    };

    Element* head;
    uint64_t size;

   public:
    /**
     * A not-thread-safe iterator
     */
    class iterator {
       private:
        ConcurrentList* list;
        Element* nextVal;

       public:
        iterator(ConcurrentList* list, bool isEnd = false) : list(list) {
            if (isEnd) {
                nextVal = nullptr;
            } else {
                nextVal = list->head;
            }
        }

        iterator& operator=(const iterator& other) {
            this->list = other.list;
            this->nextVal = other.nextVal;
            return *this;
        }

        iterator& operator++() {
            if (nextVal) {
                nextVal = nextVal->next;
            }
            return *this;
        }

        Value& operator*() {
            if (nextVal) {
                return nextVal->value;
            } else {
                std::cerr << "the iterator already reaches the end of the list"
                          << std::endl;
                abort();
            }
        }

        friend bool operator==(const iterator& l, const iterator& r) {
            return l.list == r.list && l.nextVal == r.nextVal;
        }

        friend bool operator!=(const iterator& l, const iterator& r) {
            return !(l == r);
        }
    };

   public:
    ConcurrentList() : head(nullptr), size(0) {}
    virtual ~ConcurrentList() { clear(); }

    void insert(const Value& value) {
        Element* newHead = new Element(value, nullptr);
        bool success;
        do {
            Element* oldHead = head;
            newHead->next = oldHead;
            success =
                compare_and_swap((uintptr_t*)&head, (uintptr_t)oldHead,
                                 (uintptr_t)newHead) == (uintptr_t)oldHead;
        } while (!success);
    }

    void clear() {
        Element *h = head, *next = nullptr;
        while (head) {
            next = head->next;
            delete head;
            head = next;
        }
    }

    iterator begin() { return iterator(this); }

    iterator end() { return iterator(this, true); }
};
#endif
