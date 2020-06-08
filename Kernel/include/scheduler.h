#ifndef SCHEDULER_H
#define SCHEDULER_H
#include <stddef.h>
#include <stdint.h>
#include <process.h>

extern processNode * current;

void _runProcess(uint64_t rsp);
void fakeStack(process * process);

void initScheduler();
void scheduler(uint64_t stackPointer);
void wrapper(int (*entryPoint)(int, char**), int argc, char **argv);
int getCurrentPid();


#endif
