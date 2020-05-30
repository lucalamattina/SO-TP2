#ifndef MemoryManager_h
#define MemoryManager_h

#include <stddef.h>
#include <stdint.h>
#include "lib.h"

#define MEMORY_SIZE 80 * 1048576 //80 megas
#define PAGE_SIZE 4096 //4Kb size pages
#define MAX_PAGE_QUANTITY 20480//80MB divided into 4Kb pages


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

extern freeList * free_list;
void * pmalloc(size_t space);
void pfree(void * address);
void initializeFreeList(uint64_t * free_list_address);
page * createNewPage(uint64_t * page_address, size_t allocSize, page * prev, uint64_t * dataAddress);
page * getPage(size_t size);
void splitPage(page * p, size_t usedSize);
void joinPages(page * page);

#endif
