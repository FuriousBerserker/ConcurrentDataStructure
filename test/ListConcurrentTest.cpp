#include <cinttypes>
#include <cassert>
#include <iostream>
#include <set>
#include <pthread.h>
#include "../inc/ConcurrentList.hpp"

ConcurrentList<uint32_t> ll;
std::set<uint32_t> oracle;

const int thread_num = 100;

void* insert (void* param) {
    uint32_t* data = (uint32_t*)param;
    uint32_t start = *data, end = *(data + 1), delta = *(data + 2);
    for (uint32_t i = start; i < end; i += delta) {
        ll.insert(i);        
    }
    return nullptr;
}

int main() {
    pthread_t threads[thread_num];
    cpu_set_t cpuSets[thread_num];
    uint32_t start = 5, iter = 10000, delta = 2;
    for (int i = 0; i < thread_num; i++) {
        uint32_t* data = new uint32_t[3];
        uint32_t end = start + iter * delta;
        data[0] = start;
        data[1] = end;
        data[2] = delta;
        for (uint32_t j = start; j < end; j+= delta) {
            oracle.insert(j); 
        }
        start = end;
        CPU_ZERO(&cpuSets[i]);
        CPU_SET(i % 8, &cpuSets[i]);
        pthread_create(&threads[i], NULL, insert, data);
        pthread_setaffinity_np(threads[i], sizeof(cpu_set_t), &cpuSets[i]);
    }
    for (int i = 0; i < thread_num; i++) {
        pthread_join(threads[i], NULL);
    }
    for (auto it = ll.begin(), ie = ll.end(); it != ie; ++it) {
        uint32_t element = *it;
        assert(oracle.find(element) != oracle.end());
        oracle.erase(element);
    }
    assert(oracle.empty());
    return 0;
}
