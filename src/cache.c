//========================================================//
//  cache.c                                               //
//  Source file for the Cache Simulator                   //
//                                                        //
//  Implement the I-cache, D-Cache and L2-cache as        //
//  described in the README                               //
//========================================================//

#include <stdio.h>
#include <math.h>
#include "cache.h"

//
// TODO:Student Information
//
const char *studentName = "Amardeep Ramnani";
const char *studentID   = "A59005452";
const char *email       = "aramnani@ucsd.edu";

//------------------------------------//
//        Cache Configuration         //
//------------------------------------//

uint32_t icacheSets;     // Number of sets in the I$
uint32_t icacheAssoc;    // Associativity of the I$
uint32_t icacheHitTime;  // Hit Time of the I$

uint32_t dcacheSets;     // Number of sets in the D$
uint32_t dcacheAssoc;    // Associativity of the D$
uint32_t dcacheHitTime;  // Hit Time of the D$

uint32_t l2cacheSets;    // Number of sets in the L2$
uint32_t l2cacheAssoc;   // Associativity of the L2$
uint32_t l2cacheHitTime; // Hit Time of the L2$
uint32_t inclusive;      // Indicates if the L2 is inclusive

uint32_t blocksize;      // Block/Line size
uint32_t memspeed;       // Latency of Main Memory

//------------------------------------//
//          Cache Statistics          //
//------------------------------------//

uint64_t icacheRefs;       // I$ references
uint64_t icacheMisses;     // I$ misses
uint64_t icachePenalties;  // I$ penalties

uint64_t dcacheRefs;       // D$ references
uint64_t dcacheMisses;     // D$ misses
uint64_t dcachePenalties;  // D$ penalties

uint64_t l2cacheRefs;      // L2$ references
uint64_t l2cacheMisses;    // L2$ misses
uint64_t l2cachePenalties; // L2$ penalties

//------------------------------------//
//        Cache Data Structures       //
//------------------------------------//

//Block Struct
typedef struct {
uint8_t valid;
uint32_t tag;
uint32_t lru;
} block;

block** I_cache;
block** D_cache;
block** L2_cache;

uint8_t dcache_or_icache;
uint32_t icache_per_access_time;
uint32_t dcache_per_access_time;



uint32_t find_index(uint32_t addr, uint32_t Sets);
uint32_t find_tag(uint32_t addr, uint32_t Sets);

//
//TODO: Add your Cache data structures here
//

//------------------------------------//
//          Cache Functions           //
//------------------------------------//

// Initialize the Cache Hierarchy
//
void
init_cache()
{
  // Initialize cache stats
  icacheRefs        = 0;
  icacheMisses      = 0;
  icachePenalties   = 0;
  dcacheRefs        = 0;
  dcacheMisses      = 0;
  dcachePenalties   = 0;
  l2cacheRefs       = 0;
  l2cacheMisses     = 0;
  l2cachePenalties  = 0;
  
  //
  //TODO: Initialize Cache Simulator Data Structures
  //
  icache_per_access_time = 0;
  dcache_per_access_time = 0;

  int i = 0;
  int j = 0;

  I_cache = (block**)malloc(icacheSets * sizeof(block*));
  D_cache = (block**)malloc(dcacheSets * sizeof(block*));
  L2_cache = (block**)malloc(l2cacheSets * sizeof(block*));

  for (i = 0; i < icacheSets; i++)
      I_cache[i] = (block*)malloc(icacheAssoc * sizeof(block));

  for (i = 0; i < dcacheSets; i++)
      D_cache[i] = (block*)malloc(dcacheAssoc * sizeof(block));

  for (i = 0; i < l2cacheSets; i++)
      L2_cache[i] = (block*)malloc(l2cacheAssoc * sizeof(block));

  for (i = 0; i < icacheSets; i++) {
        for (j = 0; j < icacheAssoc; j++) {
        I_cache[i][j].tag = 0;
        I_cache[i][j].lru = 0;
        I_cache[i][j].valid = 0;
        }
  }

  for (i = 0; i < dcacheSets; i++) {
        for (j = 0; j < dcacheAssoc; j++) {
        D_cache[i][j].tag = 0;
        D_cache[i][j].lru = 0;
        D_cache[i][j].valid = 0;
        }
  }

  for (i = 0; i < l2cacheSets; i++) {
        for (j = 0; j < l2cacheAssoc; j++) {
        L2_cache[i][j].tag = 0;
        L2_cache[i][j].lru = 0;
        L2_cache[i][j].valid = 0;
        }
  }
}

// Perform a memory access through the icache interface for the address 'addr'
// Return the access time for the memory operation
//

uint32_t find_index(uint32_t addr, uint32_t Sets) {
uint32_t mask;
uint32_t offset_plus_index = log2(blocksize) + log2(Sets);

mask = (1 << offset_plus_index) - 1;
uint32_t index_addr = addr & mask;
uint32_t index = index_addr >> (int) log2(blocksize);

return index;
}

uint32_t find_tag(uint32_t addr, uint32_t Sets) {
uint32_t mask;
uint32_t offset_plus_index = log2(blocksize) + log2(Sets);

uint32_t tag = addr >> offset_plus_index;

return tag;
}


uint32_t
icache_access(uint32_t addr)
{
  //
  //TODO: Implement I$
  //
  icacheRefs++;

  uint32_t index = find_index(addr, icacheSets);
  uint32_t tag = find_tag(addr, icacheSets);
  
  uint32_t counter;
  uint32_t update_lru_bound;

  
  int i;
  int match = 0;
  int find_entry = 0;
  int find_lru = 0;
  
  
  for (i = 0; i < icacheAssoc; i++) {
  if (I_cache[index][i].tag == tag && I_cache[index][i].valid == 1 && match == 0) {     
    match = 1;
    counter = i;
    update_lru_bound = I_cache[index][i].lru;
    icache_per_access_time = icacheHitTime;
   }
  }

  if (match == 0) {
     icacheMisses++;
     icachePenalties = icachePenalties + l2cacheHitTime;
     icache_per_access_time = icacheHitTime + l2cacheHitTime;

  for (i = 0; i < icacheAssoc; i++) {
  if (I_cache[index][i].valid == 0 && find_entry == 0){
     find_entry = 1;
     counter = i;
     update_lru_bound = I_cache[index][i].lru;
  }
}
  if (find_entry == 1) {
   I_cache[index][counter].valid = 1;
   I_cache[index][counter].tag = tag;
  }

  else {
    for (i = 0; i < icacheAssoc; i++) {
    if (I_cache[index][i].lru == 0 && find_lru == 0) {
       find_lru = 1;
       counter = i;
       update_lru_bound = I_cache[index][i].lru;
    }
  }
   I_cache[index][counter].valid = 1;
   I_cache[index][counter].tag = tag;

 }
}

//LRU update
for (i = 0; i < icacheAssoc; i++) {
if (I_cache[index][i].lru > update_lru_bound) {
  I_cache[index][i].lru--;
 }
}

I_cache[index][counter].lru = icacheAssoc - 1;

if (match == 0) {
dcache_or_icache = 0;
l2cache_access(addr);
}

return icache_per_access_time;

}

// Perform a memory access through the dcache interface for the address 'addr'
// Return the access time for the memory operation
//
uint32_t
dcache_access(uint32_t addr)
{
  //
  //TODO: Implement D$
  //

  dcacheRefs++;

  uint32_t index = find_index(addr, dcacheSets);
  uint32_t tag = find_tag(addr, dcacheSets);
  
  uint32_t counter;
  uint32_t update_lru_bound;
  

  
  int i;
  int match = 0;
  int find_entry = 0;
  int find_lru = 0;
  
  
  for (i = 0; i < dcacheAssoc; i++) {
  if (D_cache[index][i].tag == tag && D_cache[index][i].valid == 1 && match == 0) {     
    match = 1;
    counter = i;
    update_lru_bound = D_cache[index][i].lru;
    dcache_per_access_time = dcacheHitTime;

   }
  }

  if (match == 0) {
     dcacheMisses++;
     dcachePenalties = dcachePenalties + l2cacheHitTime;
     dcache_per_access_time = l2cacheHitTime + dcacheHitTime;


  for (i = 0; i < dcacheAssoc; i++) {
  if (D_cache[index][i].valid == 0 && find_entry == 0){
     find_entry = 1;
     counter = i;
     update_lru_bound = D_cache[index][i].lru;
  }
}
  if (find_entry == 1) {
   D_cache[index][counter].valid = 1;
   D_cache[index][counter].tag = tag;
  }

  else {
    for (i = 0; i < dcacheAssoc; i++) {
    if (D_cache[index][i].lru == 0 && find_lru == 0) {
       find_lru = 1;
       counter = i;
       update_lru_bound = D_cache[index][i].lru;
    }
  }
   D_cache[index][counter].valid = 1;
   D_cache[index][counter].tag = tag;

 }
}

//LRU update
for (i = 0; i < dcacheAssoc; i++) {
if (D_cache[index][i].lru > update_lru_bound) {
  D_cache[index][i].lru--;
 }
}

D_cache[index][counter].lru = dcacheAssoc - 1;

if (match == 0) {
dcache_or_icache = 1;
l2cache_access(addr);
}

return dcache_per_access_time;
}

// Perform a memory access to the l2cache for the address 'addr'
// Return the access time for the memory operation
//
uint32_t
l2cache_access(uint32_t addr)
{
  //
  //TODO: Implement L2$
  //


  l2cacheRefs++;

  uint32_t index = find_index(addr, l2cacheSets);
  uint32_t tag = find_tag(addr, l2cacheSets);
  
  uint32_t counter;
  uint32_t update_lru_bound;
  
  int i;
  int match = 0;
  int find_entry = 0;
  int find_lru = 0;
  
  
  for (i = 0; i < l2cacheAssoc; i++) {
  if (L2_cache[index][i].tag == tag && L2_cache[index][i].valid == 1 && match == 0) {     
    match = 1;
    counter = i;
    update_lru_bound = L2_cache[index][i].lru;
   }
  }

  if (match == 0) {
     l2cacheMisses++;
     l2cachePenalties = l2cachePenalties + memspeed;

     if(dcache_or_icache == 0) {
      icachePenalties = icachePenalties + memspeed;
      icache_per_access_time = icache_per_access_time + memspeed;
     }

     else {
      dcachePenalties = dcachePenalties + memspeed;
      dcache_per_access_time = dcache_per_access_time + memspeed;
     }

  for (i = 0; i < l2cacheAssoc; i++) {
  if (L2_cache[index][i].valid == 0 && find_entry == 0){
     find_entry = 1;
     counter = i;
     update_lru_bound = L2_cache[index][i].lru;
  }
}
  if (find_entry == 1) {
   L2_cache[index][counter].valid = 1;
   L2_cache[index][counter].tag = tag;
  }

  else {
    for (i = 0; i < l2cacheAssoc; i++) {
    if (L2_cache[index][i].lru == 0 && find_lru == 0) {
       find_lru = 1;
       counter = i;
       update_lru_bound = L2_cache[index][i].lru;
    }
  }
   L2_cache[index][counter].valid = 1;
   L2_cache[index][counter].tag = tag;

  }
 }


//LRU update
for (i = 0; i < l2cacheAssoc; i++) {
if (L2_cache[index][i].lru > update_lru_bound) {
  L2_cache[index][i].lru--;
 }
}

L2_cache[index][counter].lru = l2cacheAssoc - 1;


  return memspeed;

}
