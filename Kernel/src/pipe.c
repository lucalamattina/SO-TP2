#include <pipe.h>


pipe * pipeList[MAX_PIPES] = {NULL};

int newPipe(char * name){
  int firstEmpty = -1;
  for(int i=0; i<MAX_PIPES;i++){
      if(pipeList[i] != NULL && !strcmp(pipeList[i]->name, name)){//si el pipe ya existe, me lo devuelve
        return pipeList[i]->fd;//                                   sino busco el primer lugar vacio y retorno ese
      }
      if(firstEmpty == -1 && pipeList[i] == NULL){
        firstEmpty = i;
      }
  }
  if(firstEmpty == -1){
    return -1;
  }
  //si encontre un lugar vacio, inicializo el pipe
  pipeList[firstEmpty] = pmalloc(sizeof(pipe));
  pipeList[firstEmpty]->name = name;
  pipeList[firstEmpty]->fd = firstEmpty + 2;
  pipeList[firstEmpty]->usingPipe = 0;
  pipeList[firstEmpty]->waitingPid = -1;
  for(int j = 0; j < MAX_BUFF_SIZE; j++){
    pipeList[firstEmpty]->buffer[j] = 0;
  }

  return pipeList[firstEmpty]->fd;//devuelvo el file descriptor de mi pipe que sera el que use mi proceso para acceder al buffer

}

void readFromPipe(int pIndex, char * target){
  pipe * myPipe = pipeList[pIndex];
  if(myPipe->usingPipe){//si el pipe esta en uso, bloqueo y espero a que me desbloqueen para poder acceder al buffer compartido
    current->process->state = BLOCKED;
    myPipe->waitingPid = current->process->pid;
    _interrupt();//corto la ejecucion y obligo a correr scheduler denuevo, cambiando de proceso
  }
  myPipe->waitingPid = -1;
  myPipe->usingPipe = 1;
  strcpy(target, myPipe->buffer);//leo del buffer del pipe
  myPipe->usingPipe = 0;
  if(myPipe->waitingPid > 0){//si tengo un proceso esperando para ser desbloqueado
    process * aux = getProcess(myPipe->waitingPid);
    aux->state = READY;
  }
}

void writeToPipe(int pIndex, char * src){
  pipe * myPipe = pipeList[pIndex];
  if (myPipe->usingPipe) {//si el pipe esta en uso, bloqueo y espero a que me desbloqueen para poder acceder al buffer compartido
    current->process->state = BLOCKED;
    myPipe->waitingPid = current->process->pid;
    _interrupt();//corto la ejecucion y obligo a correr scheduler denuevo, cambiando de proceso
  }
  myPipe->waitingPid = -1;
  myPipe->usingPipe = 1;
  strcpy(myPipe->buffer, src);//escribo en el buffer del pipe
  myPipe->usingPipe = 0;
  if(myPipe->waitingPid > 0){//si tengo un proceso esperando para ser desbloqueado
    process * aux = getProcess(myPipe->waitingPid);
    aux->state = READY;
  }

}

void closePipe(int pIndex){
  for(int j = 0; j < MAX_BUFF_SIZE; j++){
    //lleno el buffer de 0s por las dudas para que no quede info en el buffer
    pipeList[pIndex]->buffer[j] = 0;
  }
  pfree(pipeList[pIndex]);
  //lo pongo en null asi se detecta como libre en el futuro
  pipeList[pIndex] = NULL;
}

void printPipes(){
  print("---------------------------\n");
  for (size_t i = 0; i < MAX_PIPES; i++) {
    if(pipeList[i] != NULL){
      print("Pipe name: %s", pipeList[i]->name);
      print("Pipe in use: %s", pipeList[i]->usingPipe ? "yes" : "no");
      print("Pipe's buffer: %s", pipeList[i]->buffer);
      print("PID of process waiting to access pipe: %d", pipeList[i]->waitingPid);
      print("Pipe fd: %d", pipeList[i]->fd);
    }
    print("---------------------------\n");
  }

}
