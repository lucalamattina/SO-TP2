#include <process.h>

static processList* processList;
static void addProcessList(process * process);
static processNode * removeProcessList(processNode * node, process * process);

void initList(void){
	processList* processList = malloc(sizeof(processList));
	processList->first = NULL;
	processList->last = NULL;
	processList->pidCount = 0;
}

process * newProcess(char * name, int argc, char** argv, int priority, int (*addressFunction) (int, char **)){
	if(processList->pidCount >= MAX_PROCESSES){
		return NULL;
	}
	process * process = malloc(sizeof(process));
	process->name = name;
	process->pid = processList->pidCount++;
	process->addressFunction = addressFunction;
	process->priority = priority
	process->state = READY;
	process->argc = argc;
	process->argv = argv;
	process->fileDescriptors[0] = 0;
	process->fileDescriptors[1] = 1;
	for (int i = 2; i <MAX_FD; i++){
		process->fileDescriptors[i] = -1;
	}
	process->stackBase = (uint64_t) malloc(PROCESS_SIZE) + PROCESS_SIZE -1;
	process->stackPointer = process->stackBase - PROCESS_SIZE +1;
	addProcess(process);
	return process;
}

static void addProcess(process * process){
	processNode * aux = malloc(sizeof(processNode));
	aux->process = process;
	if(processList->first == NULL{
		processList->first = aux;
		aux->next = NULL;
		aux->prev = NULL;
	}
	else{
		processNode* last = processList->first;
		while(last->next != NULL){
			last = last->next;
		}
	last->next = aux;
	aux->next = NULL;
	aux->prev = last;
	}
	processList->last = aux;
}


void removeProcess(int pid){
	processNode* aux = processList->first;
	while(aux != NULL && aux->pid != pid){
		aux = aux->next;
	}
	if(aux == NULL){
		return;
	}
	if(aux->pid == pid){
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

void freeNode(processNode * node){
	free(node->process->stackPointer);
	free(node->process);
	free(node);
}
