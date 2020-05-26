#ifndef PROCESS_H
#define PROCESS_H

#include <lib.h>
#include <stdlib.h>
#include <stddef.h>

#define PROCESS_SIZE 4096
#define MAX_FD 2
#define MAX_PROCESSES 20
#define FOREGROUND 1
#define BACKGROUND 0



typedef enum processState {READY, BLOCKED, RUNNING, DEAD} processState;


typedef struct processNode{
	struct process * process;
	struct processNode * next;
	struct processNode * prev;
} processNode;

typedef struct process{
	int argc;
	char **argv;
	char * name;
	long int pid;
	enum processState state;
	int priority;
	int isForeground;
	int fileDescriptors[MAX_FD];
	uint64_t stackBase;
	uint64_t stackTop;
	uint64_t stackPointer;
	int (*entryPoint) (int, char **);
}process;

typedef struct processList{
	struct processNode * first;
	struct processNode * last;
	int pidCount;
} processList;

static processList * pList;

#endif
