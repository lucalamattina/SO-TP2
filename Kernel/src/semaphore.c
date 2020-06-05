#include <semaphore.h>

sem * semList[MAX_SEM_COUNT];

void initializeSemList(){
    for(int i=0; i<MAX_SEM_COUNT; i++){
        semList[i] = NULL;
    }
}

int getAvailableSemPos(){
    for(int i=0; i<MAX_SEM_COUNT;i++){
        if(semList[i]==NULL){
            return i;
        }
    }
    return -1;
}

int getAvailableProcessPos(sem* semaforo){
    for(int i=0; i<MAX_PROCESS_COUNT;i++){
        if(semaforo->processList[i]==-1){
            return i;
        }
    }
    return -1;
}

sem * semOpen(char* name){
    int pos = getAvailableSemPos();
    if(pos == -1){
        return NULL;
    }
    semList[pos]=pmalloc(sizeof(sem));
    semList[pos]->name=name;
    semList[pos]->state=1;
    for(int i=0; i<MAX_PROCESS_COUNT; i++){
        semList[pos]->processList[i] = -1;
    }
    return semList[pos];
}

void semClose(sem * semaforo){
    for(int i=0; i<MAX_SEM_COUNT; i++){
        if(semList[i] != NULL && semList[i]==semaforo){
            pfree(semList[i]);
        }
    }
}

void semPost(sem* semaforo){
    (semaforo->state)+=1;
    int i;

    for(i = 0; i<MAX_PROCESS_COUNT && semaforo->processList[i]==-1; i++){
    }
    if(i<MAX_PROCESS_COUNT){
        setState(semaforo->processList[i], READY);
        semaforo->processList[i]=-1;
    }
}

void semWait(sem* semaforo){
    if(semaforo->state>0){
        semaforo->state-=1;
        return;
    }
    if(semaforo->state==0){
        int pos = getAvailableProcessPos(semaforo);
        if(pos == -1){
            return;
        }
        semaforo->processList[pos] = current->process->pid;
        setState(semaforo->processList[pos], BLOCKED);
    }
}