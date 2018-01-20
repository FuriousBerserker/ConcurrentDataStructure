#ifndef __CONCURRENT_HASH_MAP_H__
#define __CONCURRENT_HASH_MAP_H__

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

/**
 * DefaultHash use variable's address as its hash
 */
template <typename V>
class DefaultHash {
   public:
    static uint64_t hash(const V& var) {
        return reinterpret_cast<uint64_t>(&var);
    }
};

/**
 * IdentityHash use the variable's value as its hahs
 */
template <typename V>
class IdentityHash {
   public:
    static V hash(const V& var) { return var; }
};
/**
 * Close-address concurrent hashmap
 */
template <typename K, typename V, V NV, typename H = DefaultHash<K> >
class ConcurrentHashMap {
   public:
    typedef K Key;
    typedef V Value;
    typedef H Hash;

   private:
    struct Element {
        Key key;
        Value value;
        Element* next;
        Element(const Key& key, const Value& value, Element* next)
            : key(key), value(value), next(next) {}
        virtual ~Element() {}
    };

    uint64_t bucketNum;
    Element** buckets;
    uint64_t size;

   public:
    ConcurrentHashMap(uint64_t bucketNum) : bucketNum(bucketNum), size(0) {
        buckets = new Element*[bucketNum]();
    }

    virtual ~ConcurrentHashMap() {
        for (uint64_t i = 0; i < bucketNum; i++) {
            Element* head = buckets[i];
            while (head) {
                Element* next = head->next;
                delete head;
                head = next;
            }
        }
        delete[] buckets;
    }

    Value get(const Key& key) {
        uint64_t hash = Hash::hash(key) % bucketNum;
        Element* head = buckets[hash];
        Value result(NV);
        while (head) {
            if (head->key == key) {
                result = head->value;
                break;
            }
            head = head->next;
        }
        return result;
    }

    bool contain(const Key& key) { return get(key) != NV ? true : false; }

    /**
     * Use lightweight compare_and_swap ops to synchronize with other concurrent
     * read / write
     */
    void put(const Key& key, const Value& value) {
        uint64_t hash = Hash::hash(key) % bucketNum;
        Element* newHead = new Element(key, value, nullptr);
        bool success;
        do {
            Element* oldHead = buckets[hash];
            newHead->next = oldHead;
            success =
                compare_and_swap((uintptr_t*)&buckets[hash], (uintptr_t)oldHead,
                                 (uintptr_t)newHead) == (uintptr_t)oldHead;
        } while (!success);
    }

    ///**
    //* Attempt to insert the key if absent
    //*/
    // bool tryPut(const Key &key, const Value &value) {
    // uint64_t hash = Hash::hash(key) % bucketNum;
    // Element* head = buckets[hash];
    // bool success = false;
    // while (head) {
    // if (head->key == key) {
    // break;
    //}
    // head = head->next;
    //}
    //}

    uint64_t getSize() { return size; }

   public:
    /**
     * A not-thread-safe iterator, it is the programmer's responsibility to
     * synchronize the operations to the iterator with other operations
     */
    class iterator {
       private:
        ConcurrentHashMap* map;
        uint64_t pos;
        Element* nextVal;

       private:
        void findNextElement() {
            while (pos < map->bucketNum && map->buckets[pos] == nullptr) {
                pos++;
            }
            if (pos < map->bucketNum) {
                nextVal = map->buckets[pos];
                pos++;
            } else {
                nextVal = nullptr;
            }
        }

       public:
        iterator(ConcurrentHashMap* map, bool isEnd = false)
            : map(map), pos(0), nextVal(nullptr) {
            if (isEnd) {
                pos = map->bucketNum;
            } else {
                findNextElement();
            }
        }

        virtual ~iterator() {}

        iterator& operator=(const iterator& other) {
            this->map = other.map;
            this->pos = other.pos;
            this->nextVal = other.nextVal;
            return *this;
        }

        iterator& operator++() {
            if (nextVal) {
                if (nextVal->next) {
                    nextVal = nextVal->next;
                } else {
                    findNextElement();
                }
            }
            return *this;
        }

        std::pair<Key, Value> operator*() {
            if (nextVal) {
                return std::make_pair(nextVal->key, nextVal->value);
            } else {
                std::cerr << "the iterator already reaches the end of the map"
                          << std::endl;
                abort();
            }
        }

        friend bool operator==(const iterator& l, const iterator& r) {
            return l.map == r.map && l.pos == r.pos;
        }

        friend bool operator!=(const iterator& l, const iterator& r) {
            return !(l == r);
        }
    };

   public:
    iterator begin() { return iterator(this); }

    iterator end() { return iterator(this, true); }
};

#endif
