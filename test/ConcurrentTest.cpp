#include <pthread.h>
#include <iostream>
#include <cassert>
#include "../inc/ConcurrentHashMap.hpp"

const int NULL_VAL = -1;
ConcurrentHashMap<unsigned, unsigned, NULL_VAL, IdentityHash<unsigned> > ccMap1(10);
//ConcurrentHashMap<int, int, NULL_VAL> ccMap2(10000);
void* t1(void* params) {
    unsigned* data = (unsigned*)params;
    unsigned base = *data;
    unsigned delta = *(data + 1);
    unsigned section = *(data + 2);
    for (unsigned i = 0; i < section; i++) {
        ccMap1.put(base, base);
        base += delta;
   }
}

int main() {
    const unsigned thread_num = 100;
    unsigned k_base = 5;
    unsigned delta = 10;
    unsigned section = 1000;
    pthread_t threads[thread_num];
    for (unsigned i = 0; i < thread_num; i++) {
        unsigned* data = new unsigned[3]();
        data[0] = k_base + i * delta * section;
        data[1] = delta;
        data[2] = section;
        pthread_create(&threads[i], NULL, t1, data);
    }
    for (unsigned i = 0; i < thread_num; i++) {
        pthread_join(threads[i], NULL);
    }
    for (unsigned i = 0; i < thread_num * section; i++) {
        unsigned key = (k_base + i * delta);
        assert(ccMap1.get(key) == key);
    }
    return 0;
}

