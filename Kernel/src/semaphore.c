#include <semaphore.h>



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

int getAvailableProcessPos(*sem semaforo){
    for(int i=0; i<MAX_PROCESS_COUNT;i++){
        if(semaforo->processList[i]==-1){
            return i;
        }
    }
    return -1;
}

sem * semOpen(char* name){
    int pos = getAvailablePos();
    if(pos == -1){
        return NULL;
    }
    semList[pos]=pmalloc(sizeof(sem));
    semList[pos]->name=name;
    semList[pos]->state=0;
    for(int i=0; i<MAX_PROCESS_COUNT; i++){
        semList[pos]->processList[i] = -1;
    }
}

void semClose(sem * semaforo){
    for(int i=0; i<MAX_SEM_COUNT; i++){
        if(semList[i] != NULL && semList[i]==semaforo){
            free(semList[i]);
            return;
        }
    }
}

void semPost(sem* semaforo){
    (semaforo->state)+=1;
    int i;
    for( i = 0; semaforo->processList[i] != current->process->pid; i++){
    }
    semaforo->processList[i] = -1;
    if(semaforo->processList[0]!=-1){
        int pos = -1;
        for(int j=0; MAX_PROCESS_COUNT; j++){
            if(semaforo->processList[j] != -1){
                pos = j;
            }
        }
        if(pos != -1){
            setState(semaforo->processList[pos], READY);
        }
    }
}

void semWait(sem* semaforo){
    if(semaforo->state>0){
        semaforo->state-=1;
        
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


