#ifndef MemoryManager_h
#define MemoryManager_h

#include <stddef.h>
#include <stdint.h>
#include "lib.h"


void * malloc(size_t space);
void free(void * address);


#define MEMORY_SIZE 1073741824 //1 Gb
#define PAGE_SIZE 4096 //4Kb size pages
#define MAX_PAGE_QUANTITY 262144 //1 Gb divided into 4Kb pages


typedef struct freeList{
  struct page * head;
  struct page * tail;
  size_t freePages;
  size_t totalPages;
}freeList;

typedef struct page{
  struct * page next;
  struct * page prev;
  int free;
  uint64_t * data_address;
  size_t size;
}page;

 #endif
