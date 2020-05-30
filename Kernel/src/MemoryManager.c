#include <MemoryManager.h>


freeList * free_list;


void initializeFreeList(uint64_t * free_list_address){
  //inicializo la free_list
  free_list = (freeList *) free_list_address;
  free_list->head = NULL;
  free_list->tail = NULL;
  free_list->freePages = 0;
  free_list->totalPages = 0;

  //creo una nueva pagina ubicada de forma contigua a la lista, y dejo lugar para todas las demas paginas, por eso apunto el data_address al "final" de toda la lista
  page * newPage = createNewPage(free_list_address + sizeof(freeList), PAGE_SIZE, NULL, free_list_address + sizeof(freeList) + sizeof(page) * MAX_PAGE_QUANTITY);

}

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
  } else {
    free_list->tail->next = newPage;
    free_list->tail = newPage;
  }

  (free_list->freePages)++;
  (free_list->totalPages)++;

  return newPage;

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
  //si todas las paginas estan ocupadas o no da el tamaño para almacenar size_t y
  //la ultima pagina esta libre empiezo usando esa y veo si hace falta agregar mas
  if (free_list->tail->free && size <= free_list->tail->size) {
    currPage = free_list->tail;
    currPage->size = size;
  } else {
    currPage = createNewPage((uint64_t *)(free_list->tail + sizeof(page)), size, free_list->tail, (uint64_t *)(free_list->tail->data_address + free_list->tail->size));
  }

  currPage->free = 0;
  (free_list->freePages)--;

  return currPage;

}

void splitPage(page * p, size_t usedSize){
  //no tengo mas espacio para agregar una page
  if (free_list->totalPages >= MAX_PAGE_QUANTITY) {
    return;
  }

  if(p->size > usedSize){

    int freeSize = p->size - usedSize;
    page * auxPage = p->next;
    p->next = createNewPage((uint64_t *)(free_list->tail + sizeof(page)), freeSize, p, (uint64_t *)(p->data_address + usedSize));

    p->next->next = auxPage;
    p->size = usedSize;
    if(auxPage != NULL){
      auxPage->prev = p->next;
    }


    if(p == free_list->tail){

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


void pfree(void * data_address){
  page * currPage = free_list->head;
  while(currPage != NULL && currPage->data_address != data_address){
    currPage = currPage->next;
  }
  if (currPage == NULL || currPage->free) {
    return;
  }
  currPage->free = 1;
  (free_list->freePages)++;
  joinPages(currPage);

}

void * pmalloc(size_t size){
  page * returnPage = getPage(size);
  splitPage(returnPage, size);
  return returnPage->data_address;
}
