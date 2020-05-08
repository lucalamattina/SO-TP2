#ifdef process_h
#define process_h

#include <lib.h>
#include <stdlib.h>
#include <stddef.h>

#define PROCESS_SIZE 4096
#define MAX_FD 20
#define MAX_PROCESSES 20



typedef enum processState {READY, BLOCKED, RUNNING, DEAD} processState;

typedef struct processList{
	processNode * first;
	processNode * last;
	int pidCount;
} processList;

typedef struct processNode{
	struct Process * process;
	struct processNode * next;
	struct processNode * prev;
} processNode;

typedef struct process{
	int argc;
	char **argv;
	char * name;
	long int pid;
	processState state;
	int priotiry;
	int fileDescriptors[MAX_FD];
	uint64_t stackBase;
	uint64_t stackPointer;
	int (*addressFunction) (int, char **);
}process;
