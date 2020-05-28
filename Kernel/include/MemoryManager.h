#ifndef MemoryManager_h
#define MemoryManager_h

#include <stddef.h>
#include <stdint.h>
#include "lib.h"

#define MEMORY_SIZE 134217728 //128 megas
#define PAGE_SIZE 4096 //4Kb size pages
#define MAX_PAGE_QUANTITY 32768 //1 Gb divided into 4Kb pages

void * pmalloc(size_t space);
void pfree(void * address);

typedef struct page{
  struct page * next;
  struct page * prev;
  int free;
  uint64_t * data_address;
  size_t size;
}page;

typedef struct freeList{
  struct page * head;
  struct page * tail;
  size_t freePages;
  size_t totalPages;
}freeList;



 #endif
