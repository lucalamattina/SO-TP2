#include "MemoryManager.h"


static void * free_list_address = (void *) 0x40000;
static freeList * free_list;


void initializeFreeList(){
  //inicializo la free_list
  free_list = (freeList *) free_list_address;
  free_list->head = NULL;
  free_list->tail = NULL;
  free_list->freePages = 0;
  free_list->totalPages = 0;

  //creo una nueva pagina ubicada de forma contigua a la lista, y dejo lugar para todas las demas paginas, por eso apunto el data_address al "final" de toda la lista
  page * newPage = createNewPage(free_list_address + sizeof(freeList), PAGE_SIZE, NULL, free_list_address + sizeof(freeList) + sizeof(page) * (MAX_PAGE_QUANTITY - 1);



}

//crea una pagina nueva ubicada en page_address de tamaño size que maneja los datos de data_address. Tambien agrega la nueva page al final de la free_list
page * createNewPage(uint64_t * page_address, size_t size, page * prev, uint64_t * data_address){
  //no hay espacio para crear una pagina nueva
  if (free_list->totalPages == MAX_PAGE_QUANTITY){
    return NULL;
  }

  page * newPage = (page *) page_address;

  newPage->next = NULL;
  newPage->prev = prev;
  newPage->free = 1;
  newPage->data_address = data_address;
  newPage->size = size;

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
  size_t allocSize = size;
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
  if (free_list->tail->free) {
    currPage = free_list->tail;
  } else {
    currPage = createNewPage(free_list->tail + sizeof(page), PAGE_SIZE, free_list->tail, free_list->last->data_address + free_list->size);
  }

  currPage->free = 0;
  allocSize -= currPage->size;
  (free_list->freePages)--;
  while (allocSize > 0) {
    page * aux = createNewPage(free_list->tail + sizeof(page), PAGE_SIZE, free_list->tail, free_list->last->data_address + free_list->size);
    if (aux == NULL){
      return NULL;
    }
    currPage->size += PAGE_SIZE;
    allocSize -= PAGE_SIZE;
  }
  joinPages(currPage);
  return currPage;

}

void splitPage(page * page, size_t usedSize){
  //no tengo mas espacio para agregar una page
  if (free_list->totalPages == MAX_PAGE_QUANTITY) {
    return;
  }

  if(page->size > usedSize){

    int freeSize = page->size - usedSize;
    page * aux = page->next;
    page->next = createNewPage((uint64_t *)page + sizeof(page), freeSize, page, page->data_address + page->size);
    page->next->next = aux;

    if(page == free_list->tail){

      free_list->tail = page->next;

    }

    (free_list->freePages)++;
    (free_list->totalPages)++;

  }

}

void joinPages(page * page){
  //uno las paginas vacias a partir de page
  page * currPage = page->next;
  while (currPage != NULL && currPage->free) {
    page->size += currPage->size;
    (free_list->freePages)--;
    (free_list->totalPages)--;
    currPage = currPage->next;
  }
  //llegue a la ultima pagina
  if(currPage == NULL){
    free_list->tail = page;
    page->next = NULL;
  } else if(!currPage->free){ //llegue a una pagina ocupada
    currPage->prev = page;
    page->next = currPage;
  }

  currPage = page->prev;
  uint64_t * prevAddress = page->data_address;
  //uno las paginas vacias previas a page
  while(currPage != NULL && currPage->free){
    page->size += currPage->size;
    (free_list->freePages)--;
    (free_list->totalPages)--;
    prevAddress = currPage->data_address;
    currPage = currPage->prev;
  }
  //si no cambio esta direccion... me pisa memoria referenciada por otra page?
  page->data_address = prevAddress;
  if (currPage == NULL) {
    free_list->head = page;
    page->prev = NULL;
  } else if(!currPage->free){
    currPage->next = page;
    page->prev = currPage;
  }
}


void free(void * data_address){
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

void * malloc(size_t size){
  if(free_list != (free_list *) free_list_address){
    initializeFreeList();
  }
  page * page = getPage(size);
  splitPage(page, size);
  return page->data_address;
}