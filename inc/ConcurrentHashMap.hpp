#ifndef __CONCURRENT_HASH_MAP_H__
#define __CONCURRENT_HASH_MAP_H__

#include <cinttypes>
#include <cstddef>
#include <atomic>
#include <iostream>

#ifdef __USE_GNU_CAS
#elif __USE_PIN_CAS
#include "atomic/ops.hpp"
#endif

/**
 * A wrapper for different CAS implemention, which only support integral scalar or pointer types that are 1, 2, 4 or 8 bytes in length
 */
template <typename V>
inline V compare_and_swap(V* location, V oldVal, V newVal) {
#ifdef __USE_GNU_CAS
    return __sync_val_compare_and_swap(location, oldVal, newVal); 
#elif __USE_PIN_CAS
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
 * A wrapper for memory barrier implementation
 */
void memory_barrier() {
#ifdef __USE_GNU_CAS
#elif __USE_PIN_CAS
#endif
}

/**
 * DefaultHash use variable's address as its hash 
 */
template <typename V>
class DefaultHash {
public:
    static uint64_t hash(const V &var) {
        return reinterpret_cast<uint64_t>(&var); 
    }
};

/**
 * IdentityHash use the variable's value as its hahs
 */
template <typename V>
class IdentityHash {
public:
    static V hash(const V &var) {
        return var; 
    }

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
        Element(const Key& key, const Value& value, Element* next) : key(key), value(value), next(next){}
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

    Value get(const Key &key) {
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
    
    bool contain(const Key &key) {
        return get(key) != NV ? true : false; 
    }
    
    /**
     * Use lightweight compare_and_swap ops to synchronize with other concurrent read / write
     */
    void put(const Key &key, const Value &value) {
        uint64_t hash = Hash::hash(key) % bucketNum;
        Element* newHead = new Element(key, value, nullptr);
        bool success;
        do {
            Element* oldHead = buckets[hash];
            newHead->next = oldHead;
            success = compare_and_swap((uintptr_t*)&buckets[hash], (uintptr_t)oldHead, (uintptr_t)newHead) == (uintptr_t)oldHead;
        } while (!success);
    }
  
    ///**
     //* Attempt to insert the key if absent
     //*/
    //bool tryPut(const Key &key, const Value &value) {
        //uint64_t hash = Hash::hash(key) % bucketNum;
        //Element* head = buckets[hash];
        //bool success = false;
        //while (head) {
            //if (head->key == key) {
                //break;
            //}
            //head = head->next;
        //}
    //}

    uint64_t getSize() {
        return size;
    }

};

#endif
