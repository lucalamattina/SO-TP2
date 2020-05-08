#include "shceduler.h"
#include "process.h"

static int quantum;
static processNode * current;

void _runProcess(uint64_t rsp);

void initScheduler(int (*addressFunction)(int, char **)){
  quantum = 2;
  current = NULL;
  initList();
  Process * shell = newProcess("shell",0,NULL, FOREGROUNDHP, addressFunction);
}
