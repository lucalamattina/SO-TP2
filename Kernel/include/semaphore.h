#ifndef SEMAPHORE_H
#define SEMAPHORE_H

#define MAX_SEM_COUNT 30
#define MAX_PROCESS_COUNT 20

#include <scheduler.h>
#include <lib.h>
#include <stdlib.h>
#include <stddef.h>


typedef struct sem{
    uint64_t state;
    char * name;
    int processList[MAX_PROCESS_COUNT];
    int * id;
}sem;

extern sem * semList[MAX_SEM_COUNT];

void printsem();
void initializeSemList();
int getAvailableSemPos();
int getAvailableProcessPos(sem* semaforo);
int * semOpen(char* name, int value);
void semClose(int * semid);
void semPost(int * semid);
void semWait(int * semid);


#endif
