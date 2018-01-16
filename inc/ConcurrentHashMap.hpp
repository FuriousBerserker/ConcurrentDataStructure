#ifndef __CONCURRENT_HASH_MAP_H__
#define __CONCURRENT_HASH_MAP_H__

#include <cinttypes>
#include <cstddef>
#include <atomic>
#define USE_PTHREAD_MUTEX 0
#define USE_PIN_MUTEX 0

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
    
    void put(const Key &key, const Value &value) {
        uint64_t hash = Hash::hash(key) % bucketNum;
        Element* head = buckets[hash];
        while (head) {
            if (head->key == key) {
                break;
            }
            head = head->next;
        }
        if (head) {
            head->value = value;
        } else {
            head = buckets[hash];
            Element* newHead = new Element(key, value, head);
            buckets[hash] = newHead;
            size++;
        }
    }
    
    uint64_t getSize() {
        return size;
    }

};

#endif
