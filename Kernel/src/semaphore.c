// #include <semaphore.h>
//
// sem* semList[MAX_SEM_COUNT];
//
// void initializeSemList(){
//     for(int i=0; i<MAX_SEM_COUNT; i++){
//         semList[i] = NULL;
//     }
// }
//
// int getAvailablePos(){
//     for(int i=0; i<MAX_SEM_COUNT;i++){
//         if(semList[i]==NULL){
//             return i;
//         }
//     }
//     return -1;
// }
//
// sem * semOpen(char* name){
//     int pos = getAvailablePos();
//     if(pos == -1){
//         return NULL;
//     }
//     semList[pos]=pmalloc(sizeof(sem));
//     semList[pos]->name=name;
//     semList[pos]->state=0;
//     semList[pos]->processList = NULL;
// }
//
// void semClose(sem * semaforo){
//     for(int i=0; i<MAX_SEM_COUNT; i++){
//         if(semList[i] != NULL && semList[i]==semaforo){
//             pfree(semList[i]);
//         }
//     }
// }
//
// void semPost(sem* semaforo){
//     (semaforo->state)+=1;
//     if(semaforo->processList!=NULL){
//         setState(semaforo->processList->process->pid, READY);
//         semaforo->processList = semaforo->processList->next;
//     }
// }
//
// void semWait(sem* semaforo){
//     if(semaforo->state>0){
//         semaforo->state-=1;
//         return;
//     }
//     if(semaforo->state==0){
//         processNode * aux = semaforo->processList;
//         while(aux!=NULL){
//             setState(aux->process->pid, BLOCKED);
//             aux = aux->next;
//             if(aux->next == NULL){
//                 //aux->next = currentProccess;
//             }
//         }
//     }
// }
