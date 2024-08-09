/*
 * Copyright (C) 2024 pdnguyen of the HCMC University of Technology
 */
/*
 * Source Code License Grant: Authors hereby grants to Licensee 
 * a personal to use and modify the Licensed Source Code for 
 * the sole purpose of studying during attending the course CO2018.
 */
//#ifdef MM_TLB
/*
 * Memory physical based TLB Cache
 * TLB cache module tlb/tlbcache.c
 *
 * TLB cache is physically memory phy
 * supports random access 
 * and runs at high speed
 */


#include "mm.h"
#include <stdlib.h>
#include <stdio.h>

// #define init_tlbcache(mp,sz,...) init_memphy(mp, sz, (1, ##__VA_ARGS__))
// struct tlb_entry {
//     int pid;
//     int pgnum;
//     int physical_address;
//     int valid;
// };

// // TLB cache size
// #define TLB_SIZE 0x10000

// // TLB cache array
// struct tlb_entry tlb_cache[TLB_SIZE];

// // TLB cache initialization function
// void init_tlb_cache() {
//     for (int i = 0; i < TLB_SIZE; ++i) {
//         tlb_cache[i].valid = -1;
//     }
// }
/*
 *  tlb_cache_read read TLB cache device
 *  @mp: memphy struct
 *  @pid: process id
 *  @pgnum: page number
 *  @value: obtained value
 */
static int timeFIFO;
int tlb_cache_read(struct memphy_struct * mp, int pid, int pgnum,struct tlb_entry* tlb_cache) // type 
{
   /* TODO: the identify info is mapped to 
    *      cache line by employing:
    *      direct mapped, associated mapping etc.
    */
    for (int i = 0; i < TLB_SIZE/PAGE_SIZE; ++i) {
        if (tlb_cache[i].valid==1 && tlb_cache[i].pid == pid && tlb_cache[i].pgnum == pgnum) {
            // TLB cache hit, return the physical address
            //printf("tlb_cache_read3\n");
            return tlb_cache[i].frame ;
        }
        
    }
   return -1;
}

/*
 *  tlb_cache_write write TLB cache device
 *  @mp: memphy struct
 *  @pid: process id
 *  @pgnum: page number
 *  @value: obtained value
 */
int tlb_cache_write(struct memphy_struct *mp, int pid, int pgnum, BYTE value,int frame, int swapType,struct tlb_entry* tlb_cache)
{
   /* TODO: the identify info is mapped to 
    *      cache line by employing:
    *      direct mapped, associated mapping etc.
    */
   /*
   swapType 1 == add
   swapType 2 == free
   */
   if(swapType==1){
      for (int i = 0; i < TLB_SIZE/PAGE_SIZE; ++i) {
         if (tlb_cache[i].valid==-1) {
               tlb_cache[i].pid = pid;
               tlb_cache[i].pgnum = pgnum;
               tlb_cache[i].frame = frame;
               tlb_cache[i].valid = 1;
               tlb_cache[i].time = ++timeFIFO;

               return 1;
               break;
         }
      }
      int minTime=99999999;
            int minPos=-1;
            int i;
            for(i=0;i<TLB_SIZE/PAGE_SIZE;++i){
               if(tlb_cache[i].time<minTime) {
                  minTime=tlb_cache[i].time;
                  minPos=i;
               }
            }
            if(minPos==-1) printf("Loi o delete tlb_cache_write");
            tlb_cache[i].pid = pid;
            tlb_cache[i].pgnum = pgnum;
            tlb_cache[i].frame = frame;
            tlb_cache[i].valid = 1;
            tlb_cache[i].time = ++timeFIFO;
            return 1;
   }
    else{
      for (int i = 0; i < TLB_SIZE/PAGE_SIZE; ++i) {
        if (tlb_cache[i].valid==1 && tlb_cache[i].pid == pid && tlb_cache[i].pgnum == pgnum) {
            
            tlb_cache[i].valid = -1;
            return 1;
            break;
        }
    }
    printf("loi o free tlbcache in tlb_cache_write");
    }

   return 0;
}

/*
 *  TLBMEMPHY_read natively supports MEMPHY device interfaces
 *  @mp: memphy struct
 *  @addr: address
 *  @value: obtained value
 */
int TLBMEMPHY_read(struct memphy_struct * mp, int addr, BYTE *value)
{
   if (mp == NULL)
     return -1;
   /* TLB cached is random access by native */
   *value = mp->storage[addr];
   return 0;
}


/*
 *  TLBMEMPHY_write natively supports MEMPHY device interfaces
 *  @mp: memphy struct
 *  @addr: address
 *  @data: written data
 */
int TLBMEMPHY_write(struct memphy_struct * mp, int addr, BYTE data)
{
   if (mp == NULL)
     return -1;

   /* TLB cached is random access by native */
   mp->storage[addr] = data;

   return 0;
}

/*
 *  TLBMEMPHY_format natively supports MEMPHY device interfaces
 *  @mp: memphy struct
 */


int TLBMEMPHY_dump(struct memphy_struct * mp)
{
   /*TODO dump memphy contnt mp->storage 
    *     for tracing the memory content
    */

   return 0;
}


/*
 *  Init TLBMEMPHY struct
 */
int init_tlbmemphy(struct memphy_struct *mp, int max_size)
{
   mp->storage = (BYTE *)malloc(max_size*sizeof(BYTE)); 
   //printf("tlb_storage_size:%lu\n",sizeof(mp->storage));
  
   mp->maxsz = max_size;
    //printf("tlb_storage_maxsize:%d\n",mp->maxsz);

   mp->rdmflg = 1;

   return 0;
}
void init_tlb_cache(struct tlb_entry *mp, int maxsize) {
    for (int i = 0; i < maxsize/PAGE_SIZE; ++i) {
        mp[i].valid = -1;
    }
    //printf("init_tlb_cache");
     //printf("tlb_cache_maxsize:%d\n",maxsize);
}

//#endif
