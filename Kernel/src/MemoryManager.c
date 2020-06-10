#include <MemoryManager.h>


freeList * free_list;


//crea una pagina nueva ubicada en page_address de tamaño size que maneja los datos de data_address. Tambien agrega la nueva page al final de la free_list
page * createNewPage(uint64_t * page_address, size_t allocSize, page * prev, uint64_t * dataAddress){
  //no hay espacio para crear una pagina nueva
  if (free_list->totalPages == MAX_PAGE_QUANTITY){
    return NULL;
  }

  page * newPage = (page *) page_address;

  newPage->next = NULL;
  newPage->prev = prev;
  newPage->free = 1;
  newPage->data_address = dataAddress;
  newPage->size = allocSize;

  //caso si es la primer pagina a agregar
  if (free_list->head == NULL && free_list->tail == NULL) {
    free_list->head = newPage;
    free_list->tail = newPage;
  }
  //else {
  //   free_list->tail->next = newPage;
  //   free_list->tail = newPage;
  // }

  // (free_list->freePages)++;
  // (free_list->totalPages)++;

  return newPage;

}

void createPageInTail(){
  if(free_list->totalPages == MAX_PAGE_QUANTITY)
        {
            return;
        }
    free_list->tail->next = createNewPage((uint64_t *) (free_list->tail) + sizeof(page), PAGE_SIZE, free_list->tail, free_list->tail->data_address + free_list->tail->size);
    free_list->tail = free_list->tail->next;
    (free_list->totalPages)++;
    (free_list->freePages)++;
}

void initializeFreeList(uint64_t * free_list_address){
  //inicializo la free_list
  free_list = (freeList *) free_list_address;
  free_list->head = NULL;
  free_list->tail = NULL;
  free_list->freePages = 1;
  free_list->totalPages = 1;
  free_list->memoryAvailable = MEMORY_SIZE;
  free_list->usedMemory = 0;
  //creo una nueva pagina ubicada de forma contigua a la lista, y dejo lugar para todas las demas paginas, por eso apunto el data_address al "final" de toda la lista
  createNewPage(free_list_address + sizeof(freeList), PAGE_SIZE, NULL, free_list_address + sizeof(freeList) + sizeof(page) * (MAX_PAGE_QUANTITY - 1));
  // page * newPage2 = createNewPage((uint64_t *)(free_list->tail + sizeof(page)), 4080, free_list->tail, (uint64_t *)(free_list->tail->data_address + free_list->tail->size));
  // page * newPage3 = createNewPage((uint64_t *)(free_list->tail + sizeof(page)), 4096, free_list->tail, (uint64_t *)(free_list->tail->data_address + free_list->tail->size));
  // print("----------initMM--------\n");
  // printInteger(newPage);
  // print("\n");
  // printInteger(newPage2);
  // print("\n");
  // printInteger(newPage3);
  // print("\n");
}


//dado un tamaño para almacenar, busca y devuelve la mejor opcion de pagina (first fit)
page * getPage(size_t size){

  page * currPage = free_list->head;
  int freePages = free_list->freePages;

  while (freePages) {
    if (currPage->free && size <= currPage->size) {
      (free_list->freePages)--;
      currPage->free = 0;
      return currPage;
    }
    //como no voy a tener "paginas distintas" contiguas vacias, si no alcanza el tamaño de currPage para size, sigo de largo.
    //El free es el que se va a encargar de esto, dejando una pagina de mayor tamaño libre en vez de muchas pequeñas contiguas
    //por eso no me interesa guardarme la siguiente ya que va a estar ocupada
    else if(currPage->free && size > currPage->size){
      freePages--;
      currPage = currPage->next;
    }
    else if(!currPage->free){
      currPage = currPage->next;
    }
  }

  int auxSize;
  //si todas las paginas estan ocupadas o no da el tamaño para almacenar size_t y
  //la ultima pagina esta libre empiezo usando esa y veo si hace falta agregar mas
  if (free_list->tail->free) {
    currPage = free_list->tail;

    //createNewPage((uint64_t *)free_list->tail + sizeof(page), size, free_list->tail, (uint64_t *)free_list->tail->data_address + free_list->tail->size);
  } else {
    //currPage = createNewPage((uint64_t *)free_list->tail + sizeof(page), size, free_list->tail, (uint64_t *)free_list->tail->data_address + free_list->tail->size);
    createPageInTail();
    currPage = free_list->tail;
  }
  auxSize = size - currPage->size;
  while(auxSize >= 0){
    createPageInTail();
    auxSize -= PAGE_SIZE;
  }
  currPage->free = 0;
  (free_list->freePages)--;
  joinPages(currPage);

  return currPage;

}

void splitPage(page * p, size_t usedSize){
  //no tengo mas espacio para agregar una page
  if (free_list->totalPages >= MAX_PAGE_QUANTITY) {
    return;
  }

  if(p->size > usedSize){
    int freeSize = p->size - usedSize;
    p->size = usedSize;
    page * auxPage = p->next;
    p->next = createNewPage((uint64_t *)p + sizeof(page), freeSize, p, p->data_address + p->size);
    p->next->next = auxPage;

    if(auxPage != NULL){
      // printInteger(auxPage);
      // print("\n");
      // printInteger(auxPage->prev);
      // print("\n");
      auxPage->prev = p->next;
    }

    if(p == free_list->tail)
    {
        free_list->tail = p->next;
    }

    (free_list->freePages)++;
    (free_list->totalPages)++;

  }


}

void joinPages(page * p){
  //uno las siguientes paginas vacias a partir de page
  page * currPage = p->next;
  while (currPage != NULL && currPage->free) {
    p->size += currPage->size;
    (free_list->freePages)--;
    (free_list->totalPages)--;
    currPage = currPage->next;
    currPage->prev->next = NULL;
    currPage->prev->prev = NULL;
  }
  //llegue a la ultima pagina
  if(currPage == NULL){
    free_list->tail = p;
    p->next = NULL;
  } else if(!currPage->free){ //llegue a una pagina ocupada
    currPage->prev = p;
    p->next = currPage;
  }

  currPage = p->prev;
  uint64_t * prevAddress = p->data_address;
  //uno las paginas vacias previas a page
  while(currPage != NULL && currPage->free){
    p->size += currPage->size;
    (free_list->freePages)--;
    (free_list->totalPages)--;
    prevAddress = currPage->data_address;
    currPage = currPage->prev;
    currPage->next->next = NULL;
    currPage->next->prev = NULL;
  }
  //si no cambio esta direccion... me pisa memoria referenciada por otra page?
  p->data_address = prevAddress;
  if (currPage == NULL) {
    free_list->head = p;
    p->prev = NULL;
  } else if(!currPage->free){
    currPage->next = p;
    p->prev = currPage;
  }
}


void pfree(void * dataAddress){
  page * currPage = free_list->head;
  while(currPage != NULL && currPage->data_address != dataAddress){
    currPage = currPage->next;
  }
  if (currPage == NULL || currPage->free) {
    return;
  }
  currPage->free = 1;
  (free_list->freePages)++;

  free_list->usedMemory -= currPage->size;
  free_list->memoryAvailable += currPage->size;
  joinPages(currPage);

}

void * pmalloc(size_t size){
  page * returnPage = getPage(size);
  // print("--------getPage------\n");
  // printInteger(returnPage);
  // print("\n");
  // printInteger(size);
  // print("\n");
  splitPage(returnPage, size);
  // print("--------splitPage-------\n");
  // printInteger(returnPage);
  // print("\n");
  // printInteger(size);
  // print("\n");
  free_list->usedMemory += returnPage->size;
  free_list->memoryAvailable -= returnPage->size;
  return returnPage->data_address;
}

void mem(){
  print("---------------------------\n");
  print("Total pages: %d\n", free_list->totalPages);
  print("Free pages: %d\n", free_list->freePages);
  print("Total memory size: %d\n", MEMORY_SIZE);
  print("Memory available (in bytes): %d\n", free_list->memoryAvailable);
  print("Memory in use (in bytes): %d\n", free_list->usedMemory);
  print("---------------------------\n");
}
