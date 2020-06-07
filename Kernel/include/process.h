#ifndef PROCESS_H
#define PROCESS_H

#include <lib.h>
#include <stdlib.h>
#include <stddef.h>
#include <MemoryManager.h>

#define PROCESS_SIZE 4096
#define MAX_FD 2
#define MAX_PROCESSES 100
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
	int pid;
	enum processState state;
	int priority;
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

extern processList * pList;

void initList();
process * newProcess(char * name, int argc, char** argv, int priority, int (*entryPoint) (int, char **));
void addProcess(process * process);
void removeProcess(int pid);
process * getProcess(int pid);
void setState(int pid, processState state);
void freeNode(processNode * node);
void ps();
void block(int pid);
void kill(int pid);
void nice(int pid, int priority);
#endif
