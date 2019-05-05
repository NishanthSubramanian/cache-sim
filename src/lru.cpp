//implementation of LRU.h

#include "lru.h"
#include <stdio.h>

typedef long long ll;

LRUCache::LRUCache(int numberOfSets, int blockSize, int setAssociativity) :
    Cache(numberOfSets, blockSize, setAssociativity) {

}

void LRUCache::policySpecifcAlloc() {
    lastUsed = (ll*) malloc(numberOfSets * setAssociativity * sizeof(ll)); 
}

void LRUCache::prefetch() {

}

void LRUCache::onHit(long long row) {
    lastUsed[row] = timeElapsed;
}

void LRUCache::onMissWithNoEvict(long long row) {
    lastUsed[row] = timeElapsed;
}

long long LRUCache::onMissWithEvict(long long index, long long tag, int rowBegin, int rowEnd) {
    ll min = rowBegin;
    for(ll i = rowBegin; i < rowEnd; i++) {
        if(lastUsed[i] < lastUsed[min]) {
            min = i;
        }
    }

    ll row = min; //update row to be evicted 
    lastUsed[row] = timeElapsed;

    return row;
}

void LRUCache::postfetch() {
    timeElapsed++;
}

void LRUCache::policySpecificDealloc() {
    free(lastUsed);
}

LRUCache::~LRUCache() {
    
}