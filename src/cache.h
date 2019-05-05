#pragma once

#include <cstring>
#include <vector>
#include <string>
#include <iostream>
#include <chrono>

//classes defined
class CacheLine;
class Cache;

//error handling
void printError(std::string errorMessage);

//helper functions

bool isPowerOfTwo(long long x);
int log2(long long x);
long long hexadecimalToDecimal(char hexVal[]);

//I/O
 
long long getNextAddress(); // read a line from the trace file in stdout and returns the address of memory access
void printResult(long long simulationTime, Cache &cache); // output resulf after analysis

//cache classes

class CacheLine {
    
    private:
        bool valid; // true when block is present in cache, false when no block is present
        long long tag;
        int blockSize;

    public:
        CacheLine();
        CacheLine(int blockSize);
        
        bool isValid();
        long long getTag();
        void setValid(bool valid);
        void setTag(long long tag); 
        void setBlockSize(long long blockSize);
};

class Cache {
    
    private:
        int indexSize, offsetSize;
        long long hits, misses;
        CacheLine* cacheLines;
        int* nextFreeBlockInSet;
        //set is full if nextFreeBlockInSet == setAssociativity

    public:

        bool warmedUp(); //check if the cache has been populated
        void incHits(); //Must be called when there is a cache hit
        void incMisses(); //Must be called when there is a cache miss
        
        long long isDataInCache(long long address); //returns the row in cache data is present at
        long long isCacheLineInCache(long long index, long long tag);
        bool isSetFull(long long index);
        long long insertData(long long address); //will insert data only if free cacheLines are available and returns row where data inserted
        long long insertCacheLine(long long index, long long tag);
        void evictAndInsertData(long long evictionAddress, long long insertionAddress); //A block is evicted when some other block is inserted
        void evictAndInsertCacheLine(long long eIndex, long long eTag, long long iIndex, long long iTag);
        void evictAndInsertCacheLine(long long row, long long insertionAddress); //row to be evicted, insertionAddress: address of incoming data
        
        double hitRate();
        double missRate();
        long long getNumberOfHits(); //for debugging
        long long getNumberOfMisses(); //for debugging
        long long getTagFromAddress(long long address);
        long long getIndexFromAddress(long long address);

        void displayCache(); //for debugging
        
        virtual void policySpecifcAlloc() = 0; //for eviction policy specfic allocation
        virtual void prefetch() = 0;
        virtual void onHit(long long row) = 0; //row is the row number of cacheline where data is present
        virtual void onMissWithNoEvict(long long row) = 0; //row number of cacheline where data is inserted (NOTE: This is to be changed)
        virtual long long onMissWithEvict(long long index, long long tag, int rowBegin, int rowEnd) = 0; //has to return the row number of evicted cache line (wherein new cacheline is inserted)
        virtual void postfetch() = 0;
        virtual void policySpecificDealloc() = 0; //for eviction policy specific deallocation

        virtual ~Cache(); //destructor

    protected:
        int numberOfRows; //Number of rows in cache
        int setAssociativity; //1 for direct mapped
        int numberOfSets;
        Cache(int numberOfSets, int blockSize, int setAssociativity);
};
