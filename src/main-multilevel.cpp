//cache simulation

#include <vector>
#include "lru.h" //this line also needs to be changed from cache.py (read *** first)
#include <chrono>
using namespace std;
using namespace std::chrono;

typedef long long ll;

int main(int argc, char *argv[]) {

    ll numberOfSets = atoll(argv[1]); //atoll : char* to long long
    ll blockSize = atoll(argv[2]);
    ll setAssociativity = atoll(argv[3]);
    
    LRUCache L1cache(numberOfSets, blockSize, setAssociativity); //initialise a cache with relevant parameters
    //SRRIPCache L2cache(...);
    //NFUCache L3cache(...);

    vector<Cache*> caches; //could use array for faster execution

    caches.push_back(&L1cache); //we have to append in the same order as L1, L2, ..
    //caches.push_back(L2cache);
    //caches.push_back(L3cache);

    //lru specific allocation begins
    for(int i = 0; i < caches.size(); i++) {
        caches[i]->policySpecifcAlloc();
    }
    //lru specific allocation ends 

    //measure time
    auto start = high_resolution_clock::now();

    while(true) {

        ll address = getNextAddress();
        if(address == 0) break; // reached eof

        for(int i = caches.size() - 1; i >= 0; i--) { //not sure of order of call
            caches[i]->prefetch();
        }

        bool search = true;
        int i = 0; //indicates which level of cache (0-indexed)
        while(i > -1) { 
            if(i == caches.size()) { //not found in any level of caches
                search = false; //found the data in memory
                i--;
            }
            ll row;
            if(search) { //search for data in current cache level
                if((row = caches[i]->isDataInCache(address)) != -1) { //cache hit
                    //isDataInCache returns the row number where the data is present
                    caches[i]->incHits();  //increment hits
                    caches[i]->onHit(row);
                    search = false; //no lobger need to search data since it is found
                    i--; //send data back to higer level (Convention: L1 is highest)
                }
                else {
                    i++; //search data in lower level
                }
            }
            else { //data is found at a lower level and is sent back
                caches[i]->incMisses(); //increment misses
                if(!caches[i]->isSetFull(caches[i]->getIndexFromAddress(address))) { //if empty slot is available in cache to insert data
                    row = caches[i]->insertData(address);
                    //insertDataToCache returns the row where the data is inserted
                    caches[i]->onMissWithNoEvict(row);
                }
                else { //if no empty slot is available in cache
                    ll index = caches[i]->getIndexFromAddress(address); //value of index of address
                    ll tag = caches[i]->getTagFromAddress(address); //value of tag of address
                    ll rowBegin = index * caches[i]->getSetAssociativity(); 
                    ll rowEnd = (index + 1) * caches[i]->getSetAssociativity();

                    //rows between rowBegin(inclusive) and rowEnd(exclusive) must be considered for eviction

                    //NOTE: the row number of the block to be evicted must be stored in row!!
                    row = caches[i]->onMissWithEvict(index, tag, rowBegin, rowEnd);
                
                    caches[i]->evictAndInsertCacheLine(row, address);
                }
                i--; //send data to higher level              
            }
        }

        for(int i = 0; i < caches.size(); i++) { //not sure of order of call
            caches[i]->postfetch();
        }

        //debugging begins
        //cache.displayCache();
        //debugging ends
    }

    //measure time
    auto stop = high_resolution_clock::now(); 
    auto duration = duration_cast<milliseconds>(stop - start);

    //output
    for(int i = 0; i < caches.size(); i++) {
        printf("L%d Cache:\n", i+1);
        printResult(duration.count(), *caches[i]);
        printf("\n");
    }
    
    //lru specific deallocation begins
    for(int i = 0; i < caches.size(); i++) {
        caches[i]->policySpecificDealloc();
    }
    //lru specific deallocation ends

    return 0;
}