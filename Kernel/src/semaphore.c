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

int * semOpen(char * name){
    int pos = getAvailableSemPos();
    if(pos == -1){
        return NULL;
    }
    semList[pos]=pmalloc(sizeof(sem));
    semList[pos]->name=name;
    semList[pos]->state=1;
    semList[pos]->id = pos;
    for(int i=0; i<MAX_PROCESS_COUNT; i++){
        semList[pos]->processList[i] = -1;
    }
    return &semList[pos]->id;
}

void semClose(int * semid){
    sem * semaforo = semList[*semid];
    pfree(semaforo);
    semList[*semid] = NULL;

}

void semPost(int * semid){
    sem * semaforo = semList[*semid];
    (semaforo->state)+=1;
    int i;

    for(i = 0; i<MAX_PROCESS_COUNT && semaforo->processList[i]==-1; i++){
    }
    if(i<MAX_PROCESS_COUNT){
        setState(semaforo->processList[i], READY);
        semaforo->processList[i]=-1;
    }
}

void semWait(int * semid){
    sem * semaforo = semList[*semid];
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
        _interrupt();
    }
}

void printsem(){
	print("---------------------------\n");
	for(i=0;i<MAX_SEM_COUNT;i++){
        if(semList[i]!=NULL){
		print("Name: %s \n", semList[i]->name);
		print("PID: %d \n", semList[i]->state);
        print("Blocked porcesses: { \n";
		for(int j = 0; j != MAX_PROCESS_COUNT; j++){
            if(semList[i]->processList[j]!=-1){
                print(" PID = %d \n", semList[i]->processList[j]);
            }
        }
        print("}\n");
		print("---------------------------\n");
	}
}
