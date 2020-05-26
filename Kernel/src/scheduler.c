#include "scheduler.h"

static int quantum;
static processNode * current;



void initScheduler(){
  quantum = 2;
  current = NULL;
  //initList();
  //process * shell = newProcess("shell",0,NULL, 10 ,FOREGROUND, entryPoint);
  fakeStack(pList->first->next->process); //fake shell´s stack
  current->process = pList->first->next->process;
  setState(current->process->pid, RUNNING);
  _runProcess(current->process->stackPointer);
}


void scheduler(uint64_t stackPointer){
  current->process->stackPointer = stackPointer;
  if(current->process->state == RUNNING){
    if(quantum > 0){
      quantum--;
      return;
    }
    setState(current->process->pid, READY);
  }
  do {
    if(current->next == NULL){
      current = pList->first;
    }
    else{
      current = current->next;
    }
  } while(current->process->state != READY);
  setState(current->process->pid, RUNNING);
  quantum = current->process->priority;
  _runProcess(current->process->stackPointer);
}

void fakeStack(process * process)
{
  process->stackPointer = _fakeStack(process->stackBase, process->entryPoint, process->argc, process->argv,(uint64_t)wrapper);
}

void wrapper(int (*entryPoint)(int, char**), int argc, char **argv){
  entryPoint(argc, argv);
  current->process->state = DEAD;
  _interrupt();
}
