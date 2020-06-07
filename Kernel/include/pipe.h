#include <lib.h>
#include <scheduler.h>

#define MAX_PIPES 30
#define MAX_BUFF_SIZE 100

typedef struct pipe{
    char buffer[MAX_BUFF_SIZE];
    char * name;
    int fd;
    // semaphore* semaphore; creo que tiene que ir pero todavia no esta hecha la struct de semaforo
    int usingPipe;
    int waitingPid;
}pipe;

extern pipe * pipeList[MAX_PIPES];
int newPipe(char * name);
void readFromPipe(int pIndex, char * target);
void writeToPipe(int pIndex, char * src);
void closePipe(int pIndex);
void printPipes();
