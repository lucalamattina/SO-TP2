#include <process.h>

processList * pList; //agregamos esto en el .h y .c creo que es lo que falla


void initList(void){
	pList = pmalloc(sizeof(pList));
	pList->first = NULL;
	pList->last = NULL;
	pList->pidCount = 0;
}

process * newProcess(char * name, int argc, char** argv, int priority, int (*entryPoint) (int, char **)){
	if(pList->pidCount >= MAX_PROCESSES){
		return NULL;
	}
	process * process = pmalloc(sizeof(process));
	process->name = name;
	process->pid = pList->pidCount++;
	process->entryPoint = entryPoint;
	process->priority = priority;
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

void addProcess(process * process){
	processNode * aux = pmalloc(sizeof(processNode));
	aux->process = process;
	if(pList->first == NULL){
		pList->first = aux;
		aux->next = NULL;
		aux->prev = NULL;
	}
	else{
	pList->last->next = aux;
	aux->next = NULL;
	aux->prev = pList->last;
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
	} else if(aux->process->pid == pid){
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
	if(aux != NULL){
		return aux->process;
	}
	return NULL;
}

void setState(int pid, processState state){
  process * aux = getProcess(pid);
  if(aux!=NULL && aux->pid == pid){
    aux->state = state;
  }
}

void freeNode(processNode * node){
	node->prev->next = node->next;
	if(node->next != NULL){
		node->next->prev = node->prev;
	}else{
		pList->last = node->prev;
	}
	pfree((void *)node->process->stackBase);
	pfree((void *)node->process);
	pfree((void *)node);
	node = NULL;

}

void ps(){
	processNode * aux = pList->first;
	print("---------------------------\n");
	while(aux!=NULL){
		print("Name: %s \n", aux->process->name);
		print("PID: %d \n", aux->process->pid);
		print("Priority: %d \n", aux->process->priority < 0 ? aux->process->priority * (-1) : aux->process->priority);
		print("Visibility: %s\n", aux->process->priority < 0 ? "background" : "foreground");
		print("StackPointer: %d \n", aux->process->stackPointer);
		print("BasePointer: %d \n", aux->process->stackBase);
		switch(aux->process->state){
			case READY: print("State: READY\n");break;
			case RUNNING: print("State: RUNNING\n");break;
			case BLOCKED: print("State: BLOCKED\n");break;
			case DEAD: print("State: DEAD\n");break;
		}
		print("---------------------------\n");
		aux = aux->next;
	}
}

int block(int pid){
	process * aux = getProcess(pid);
	if(aux != NULL && aux->state != DEAD){
		if(aux->state == BLOCKED){
			aux->state = READY;
		}
		else if(aux->state != BLOCKED){
			aux->state = BLOCKED;
		}
		return pid;
	}
	return -1;
}

int kill(int pid){
	if(pid == 0){
		print("You can't kill shell\n");
		return 0;
	}

	processNode * aux = pList->first;
	while( aux != NULL && aux->process->pid != pid ){
		aux = aux->next;
	}
	if(aux!=NULL && aux->process->pid == pid && aux->process->state != DEAD){
		setState(pid, DEAD);
		//pList->pidCount--;
		//freeNode(aux);
		return pid;
	}
	print("Invalid PID\n");
	return -1;

}

int nice(int pid, int priority){
	process * aux = getProcess(pid);
	if(aux != NULL && aux->state != DEAD){
		if(aux->priority < 0){
			aux->priority = priority * (-1);
		} else{
			aux->priority = priority;
		}
		return priority;
	}
	return -1;
}

void setFd(int pid, int fdToModify, int newFd){
	process * aux = getProcess(pid);
	if (aux != NULL) {
		aux->fileDescriptors[fdToModify] = newFd;
	}
}

int getFd(int pid, int fdPos){
	process * aux = getProcess(pid);
	if (aux != NULL) {
		return aux->fileDescriptors[fdPos];
	}
	return -1;
}

int getVisibility(int pid){
	process * aux = getProcess(pid);
	if (aux != NULL) {
		return aux->priority < 0 ? 0 : 1;
	}
	return -1;
}
