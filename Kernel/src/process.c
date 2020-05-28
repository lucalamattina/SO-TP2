#include <process.h>

static void addProcess(process * process);
static processNode * removeProcessList(processNode * node, process * process);

void initList(void){
	pList = pmalloc(sizeof(pList));
	pList->first = NULL;
	pList->last = NULL;
	pList->pidCount = 0;
}

process * newProcess(char * name, int argc, char** argv, int priority, int isForeground , int (*entryPoint) (int, char **)){
	if(pList->pidCount >= MAX_PROCESSES){
		return NULL;
	}
	process * process = pmalloc(sizeof(process));
	process->name = name;
	process->pid = pList->pidCount++;
	process->entryPoint = entryPoint;
	process->priority = priority;
	process->isForeground = isForeground;
	process->state = READY;
	process->argc = argc;
	process->argv = argv;
	process->fileDescriptors[0] = 0;
	process->fileDescriptors[1] = 1;
	process->stackBase = (uint64_t) pmalloc(PROCESS_SIZE) + PROCESS_SIZE -1;
	process->stackTop = process->stackBase - PROCESS_SIZE +1;
	process->stackPointer = process->stackBase;
	addProcess(process);
	return process;
}

static void addProcess(process * process){
	processNode * aux = pmalloc(sizeof(processNode));
	aux->process = process;
	if(pList->first == NULL){
		pList->first = aux;
		aux->next = NULL;
		aux->prev = NULL;
	}
	else{
		processNode* last = pList->first;
		while(last->next != NULL){
			last = last->next;
		}
	last->next = aux;
	aux->next = NULL;
	aux->prev = last;
	}
	pList->last = aux;
}


void removeProcess(int pid){
	if(pid != 0){
	processNode* aux = pList->first;
	while(aux != NULL && aux->process->pid != pid){
		aux = aux->next;
	}
	if(aux == NULL){
		return;
	}
	if(aux->process->pid == pid){
			if(aux->prev == NULL){
				aux->next->prev = NULL;
			}
			else{
				aux->prev->next = aux->next;
				if(aux->next != NULL){
					aux->next->prev = aux->prev;
				}
			}
			freeNode(aux);
	}
}
}

process * getProcess(int pid){
	processNode * aux = pList->first;
  while(aux!=NULL && aux->process->pid != pid){
    aux = aux->next;
  }
	return aux->process;
}

void setState(int pid, processState state){
  process * aux = getProcess(pid);
  if(aux!=NULL && aux->pid == pid){
    aux->state = state;
  }
}

void freeNode(processNode * node){
	pfree(node->process->stackBase);
	pfree(node->process);
	pfree(node);
	pList->pidCount--;
}

void ps(){
	processNode * aux = pList->first;
	while(aux!=NULL){
		print("Process: %s \n", aux->process->name);
		print("PID: %d \n", aux->process->pid);
		switch(aux->process->state){
			case READY: print("State: ");break;
		}
	}
}

void block(int pid){
	process * aux = getProcess(pid);
	if(aux != NULL){
			if(aux->state == BLOCKED){
				aux->state = READY;
			}
			else if(aux->state!= DEAD){
				aux->state == BLOCKED;
			}
	}
}

void kill(int pid){
	setState(pid, DEAD);
	processNode * aux = pList->first;
	while( aux != NULL && aux->process->pid != pid ){
		aux = aux->next;
	}
	if(aux!=NULL){
		freeNode(aux);
	}
}

void nice(int pid, int priority){
	process * aux = getProcess(pid);
	if(aux != NULL){
		aux->priority = priority;
	}
}