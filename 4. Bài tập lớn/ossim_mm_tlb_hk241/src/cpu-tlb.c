/*
 * Copyright (C) 2024 pdnguyen of the HCMC University of Technology
 */
/*
 * Source Code License Grant: Authors hereby grants to Licensee 
 * a personal to use and modify the Licensed Source Code for 
 * the sole purpose of studying during attending the course CO2018.
 */
//#ifdef CPU_TLB
/*
 * CPU TLB
 * TLB module cpu/cpu-tlb.c
 */

#include "mm.h"
#include <stdlib.h>
#include <stdio.h>
#ifdef CPU_TLB
// // TLB cache array
struct tlb_entry tlb_cache[TLB_SIZE];

// // TLB cache initialization function
// void init_tlb_cache() {
//     for (int i = 0; i < TLB_SIZE; ++i) {
//         tlb_cache[i].valid = -1;
//     }
// }
int tlb_change_all_page_tables_of(struct pcb_t *proc,  struct memphy_struct * mp)
{
  /* TODO update all page table directory info 
   *      in flush or wipe TLB (if needed)
   */

  return 0;
}

int tlb_flush_tlb_of(struct pcb_t *proc, struct memphy_struct * mp)
{
  /* TODO flush tlb cached*/

  return 0;
}

/*tlballoc - CPU TLB-based allocate a region memory
 *@proc:  Process executing the instruction
 *@size: allocated size 
 *@reg_index: memory region ID (used to identify variable in symbole table)
 */
int tlballoc(struct pcb_t *proc, uint32_t size, uint32_t reg_index)
{
  //printf("tlb_alloc\n");
  int addr, val;
  /* By default using vmaid = 0 */
  printf("\tAlloc at region:%d size:%d in process %d\n",reg_index,size,proc->pid);fflush(stdout);
  val = __alloc(proc, 0, reg_index, size, &addr);
  /* TODO update TLB CACHED frame num of the new allocated page(s)*/
  /* by using tlb_cache_read()/tlb_cache_write()*/
  struct vm_rg_struct *rgnode = get_symrg_byid(proc->mm, reg_index);
  printf("\tFinish alloc region=%d size=%d adrress form %ld to %ld at process %d\n",reg_index,size,rgnode->rg_start,rgnode->rg_end,proc->pid);fflush(stdout);
  return val;
}

/*pgfree - CPU TLB-based free a region memory
 *@proc: Process executing the instruction
 *@size: allocated size 
 *@reg_index: memory region ID (used to identify variable in symbole table)
 */
int tlbfree_data(struct pcb_t *proc, uint32_t reg_index)
{   
   //printf("tlb_free\n");
   printf("\tFree at region=%d at process %d\n",reg_index,proc->pid);fflush(stdout);
   struct vm_rg_struct *rgnode = get_symrg_byid(proc->mm, reg_index);
   int addr= rgnode->rg_start;
   if(addr==-1){ printf("\tBo nho chua duoc cap phat\n"); fflush(stdout);
   return -1;
   }
   int pgn = PAGING_PGN(addr);
  __free(proc, 0, reg_index);
  
  /* TODO update TLB CACHED frame num of freed page(s)*/
  /* by using tlb_cache_read()/tlb_cache_write()*/
  if(tlb_cache_read(proc->mram,proc->pid,pgn,proc->tlb_cache)!=-1){
    printf("\tco trong tlb\n");fflush(stdout);
    tlb_cache_write(proc->mram,proc->pid,pgn,0,0,0,proc->tlb_cache);
  }
  printf("\tFinish free region=%d adrress form %ld to %ld at process %d\n",reg_index,rgnode->rg_start,rgnode->rg_end,proc->pid);fflush(stdout);
  rgnode->rg_start=-1;
  return 0;
}


/*tlbread - CPU TLB-based read a region memory
 *@proc: Process executing the instruction
 *@source: index of source register
 *@offset: source address = [source] + [offset]
 *@destination: destination storage
 */
int tlbread(struct pcb_t * proc, uint32_t source,
            uint32_t offset, 	uint32_t destination,int* countTLB, int* countTLBMiss) 
{
  BYTE data; 
  int frmnum = -1;
  /* TODO retrieve TLB CACHED frame num of accessing page(s)*/
  /* by using tlb_cache_read()/tlb_cache_write()*/
  /* frmnum is return value of tlb_cache_read/write value*/
  struct vm_rg_struct *rgnode = get_symrg_byid(proc->mm, source);
  if(rgnode->rg_start==-1) {
    printf("\tRegion %d chua duoc cap phat\n",source);fflush(stdout);
    return -1;
  }
	int sourceAddress= rgnode->rg_start + offset;;
  int pgn = PAGING_PGN(sourceAddress);
  int off = PAGING_OFFST(sourceAddress);
  frmnum= tlb_cache_read(proc->mram,proc->pid,pgn,proc->tlb_cache);
  
#ifdef IODUMP
  if (frmnum >= 0){
    printf("\tTLB hit at read region=%d offset=%d\n", source, offset);fflush(stdout);
    *countTLB=*countTLB+1;
  }
  else{
    printf("\tTLB miss at read region=%d offset=%d\n",source, offset);fflush(stdout);
    *countTLB=*countTLB+1;
    *countTLBMiss=*countTLBMiss+1;
  }
  int val;
  if(frmnum>=0){
   int phyaddr = (frmnum << PAGING_ADDR_FPN_LOBIT) + off;
  TLBMEMPHY_read(proc->mram,phyaddr,&data);
  }
  else{
  val = __read(proc, 0, source, offset, &data);
   uint32_t pte = proc->mm->pgd[pgn];
   frmnum=PAGING_FPN(pte);

    tlb_cache_write(proc->mram,proc->pid,pgn,0,frmnum,1,proc->tlb_cache);
  }
  destination = (uint32_t) data;
#ifdef IODUMP
printf("\tread region=%d offset=%d value=%d process=%d\n", source, offset, data,proc->pid);fflush(stdout);
#endif

  /* TODO update TLB CACHED with frame num of recent accessing page(s)*/
  /* by using tlb_cache_read()/tlb_cache_write()*/

#ifdef PAGETBL_DUMP
  print_pgtbl(proc, 0, -1); //print max TBL
#endif
  MEMPHY_dump(proc->mram);
#endif

  return val;
}

/*tlbwrite - CPU TLB-based write a region memory
 *@proc: Process executing the instruction
 *@data: data to be wrttien into memory
 *@destination: index of destination register
 *@offset: destination address = [destination] + [offset]
 */
int tlbwrite(struct pcb_t * proc, BYTE data,
             uint32_t destination, uint32_t offset,int* countTLB, int* countTLBMiss)
{
  //printf("tlb_write\n");
  int val;
  int frmnum = -1;
  
  /* TODO retrieve TLB CACHED frame num of accessing page(s))*/
  /* by using tlb_cache_read()/tlb_cache_write()
  frmnum is return value of tlb_cache_read/write value*/
 struct vm_rg_struct *rgnode = get_symrg_byid(proc->mm, destination);
	int sourceAddress= rgnode->rg_start;
  if(sourceAddress==-1) {
    printf("\tRegion %d chua duoc cap phat\n",destination);fflush(stdout);
    return -1;
  }
  // printf("address region %d=%d\n",destination,sourceAddress);
  int pgn = PAGING_PGN(sourceAddress);
  int offAdd=sourceAddress+offset;
  int off = PAGING_OFFST(offAdd);
  // printf("pgn=%d\n",pgn);
  frmnum= tlb_cache_read(proc->mram,proc->pid,pgn,proc->tlb_cache);

#ifdef IODUMP
  if (frmnum >= 0){
    printf("\tTLB hit at write region=%d offset=%d value=%d\n",
	          destination, offset, data);fflush(stdout);
    *countTLB=*countTLB+1;
  }
	else{
    printf("\tTLB miss at write region=%d offset=%d value=%d\n",
            destination, offset, data);fflush(stdout);
    *countTLB=*countTLB+1;
    *countTLBMiss=*countTLBMiss+1;
  }
  if(frmnum>=0){
    int phyaddr = (frmnum << PAGING_ADDR_FPN_LOBIT) + off;
    TLBMEMPHY_write(proc->mram,phyaddr,data);
  }
  else{
  val = __write(proc, 0, destination, offset, data);
  uint32_t pte = proc->mm->pgd[pgn];
  frmnum=PAGING_FPN(pte);
    tlb_cache_write(proc->mram,proc->pid,pgn,0,frmnum,1,proc->tlb_cache);

  }
  /* TODO update TLB CACHED with frame num of recent accessing page(s)*/
  /* by using tlb_cache_read()/tlb_cache_write()*/
  #ifdef IODUMP
  printf("\twrite region=%d offset=%d value=%d process=%d\n", destination, offset, data,proc->pid);fflush(stdout);
  #endif
#ifdef PAGETBL_DUMP
  print_pgtbl(proc, 0, -1); //print max TBL
#endif
  MEMPHY_dump(proc->mram);
#endif
  return val;
}

#endif
