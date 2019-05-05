//cache simulation

#include "lru.h" //this line also needs to be changed from cache.py (read *** first)
#include <chrono>
using namespace std;
using namespace std::chrono;

typedef long long ll;

int main(int argc, char *argv[]) {

    ll numberOfSets = atoll(argv[1]); //atoll : char* to long long
    ll blockSize = atoll(argv[2]);
    ll setAssociativity = atoll(argv[3]);

    // *** //
    //the following one line is the only one with mention of lru 
    //perheps this can be resolved by using python (cache.py) to rewrite 
    //the following line, according to the replacement policy chosen
    LRUCache L1cache(numberOfSets, blockSize, setAssociativity); //initialise a cache with relevant parameters
    Cache &cache = L1cache;

    //lru specific allocation begins
    cache.policySpecifcAlloc();
    //lru specific allocation ends 

    //measure time
    auto start = high_resolution_clock::now();

    while(true) {

        ll address = getNextAddress();
        if(address == 0) break; // reached eof

        cache.prefetch();

        ll row; //row in cache
        if((row = cache.isDataInCache(address)) != -1) { //cache hit
            //isDataInCache returns the row number where the data is present
            cache.incHits();  //increment hits

            cache.onHit(row);
        }
        else { //cache miss
            cache.incMisses(); //increment misses
            if(!cache.isSetFull(cache.getIndexFromAddress(address))) { //if empty slot is available in cache to insert data
                row = cache.insertData(address);
                //insertDataToCache returns the row where the data is inserted
                
                cache.onMissWithNoEvict(row);

            }
            else { //if no empty slot is available in cache
                long long index = cache.getIndexFromAddress(address); //value of index of address
                long long tag = cache.getTagFromAddress(address); //value of tag of address
                long long rowBegin = index * setAssociativity; 
                long long rowEnd = (index + 1) * setAssociativity;

                //rows between rowBegin(inclusive) and rowEnd(exclusive) must be considered for eviction

                //NOTE: the row number of the block to be evicted must be stored in row!!
                row = cache.onMissWithEvict(index, tag, rowBegin, rowEnd);
                
                cache.evictAndInsertCacheLine(row, address);
            }
        }

        cache.postfetch();

        //debugging begins
        //cache.displayCache();
        //debugging ends
    }

    //measure time
    auto stop = high_resolution_clock::now(); 
    auto duration = duration_cast<milliseconds>(stop - start);

    //output
    printResult(duration.count(), cache);
    
    //lru specific deallocation begins
    cache.policySpecificDealloc();
    //lru specific deallocation ends

    return 0;
}