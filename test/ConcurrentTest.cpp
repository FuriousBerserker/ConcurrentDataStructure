#include <pthread.h>
#include <iostream>
#include <cassert>
#include "../inc/ConcurrentHashMap.hpp"

const int NULL_VAL = -1;
ConcurrentHashMap<unsigned, unsigned, NULL_VAL, IdentityHash<unsigned> > ccMap1(10000);
//ConcurrentHashMap<int, int, NULL_VAL> ccMap2(10000);
void* t1(void* params) {
   unsigned k1 = 300, delta = 20000;
   for (int i = 0; i < 1000; i++) {
        ccMap1.put(k1, k1);
        k1 += delta;
   }
}

void* t2(void* params) {
    unsigned k2 = 1300, delta = 20000;
    for (int i = 0; i < 50; i++) {
        ccMap1.put(k2, k2);
        for (int j = 0; j < 100000; j++) {
            assert(ccMap1.get(k2) == k2);
        }
        k2 += delta;
    }
}

int main() {
    pthread_t threads[21];
    for (int i = 0; i < 20; i++) {
        pthread_create(&threads[i], NULL, t1, NULL);
    }
    pthread_create(&threads[20], NULL, t2, NULL);
    for (int i = 0; i < 20; i++) {
        pthread_join(threads[i], NULL);
    }
    pthread_join(threads[20], NULL);
    return 0;
}

