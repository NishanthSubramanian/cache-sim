#pragma once

#include "cache.h"

class LRUCache : public Cache {
    private:
        long long timeElapsed;
        long long *lastUsed;
    public:
        LRUCache(int numberOfSets, int blockSize, int setAssociativity);
        
        void policySpecifcAlloc() override;
        void prefetch() override;
        void onHit(long long row) override;
        void onMissWithNoEvict(long long row) override;
        long long onMissWithEvict(long long index, long long tag, int rowBegin, int rowEnd) override;
        void postfetch() override;
        void policySpecificDealloc() override;

        ~LRUCache();
};