#include <philosophers.h>

#define MAX_PHYLO_COUNT 8
#define MIN_PHYLO_COUNT 4
#define HUNGRY 0
#define EATING 1
#define FULL 2

#define FOREGROUND 1
#define BACKGROUND 0

uint64_t * forks[MAX_PHYLO_COUNT];
int philos[MAX_PHYLO_COUNT];
int states[MAX_PHYLO_COUNT];

int currPhilCount;
int start = 1;
int * semPhilo;

void initPhilos(){
    for(int i=4; i<MAX_PHYLO_COUNT; i++){
        forks[i]=NULL;
        philos[i]=-1;
        states[i]=-1;
    }
    uint64_t * fork1 = sys_sem_open("fork1");
    uint64_t * fork2 = sys_sem_open("fork2");
    uint64_t * fork3 = sys_sem_open("fork3");
    uint64_t * fork4 = sys_sem_open("fork4");

    uint64_t  philo1 = sys_new_process("philo1", 0, NULL, 10, FOREGROUND, philo);
    uint64_t  philo2 = sys_new_process("philo2", 0, NULL, 10, FOREGROUND, philo);
    uint64_t  philo3 = sys_new_process("philo3", 0, NULL, 10, FOREGROUND, philo);
    uint64_t  philo4 = sys_new_process("philo4", 0, NULL, 10, FOREGROUND, philo);

    currPhilCount = 4;

    forks[0]=fork1;
    forks[1]=fork2;
    forks[2]=fork3;
    forks[3]=fork4;

    philos[0]=philo1;
    philos[1]=philo2;
    philos[2]=philo3;
    philos[3]=philo4;

    states[0]=0;
    states[1]=0;
    states[2]=0;
    states[3]=0;
}


int addPhilo(){
    if(currPhilCount >= MAX_PHYLO_COUNT){
        return 0;
    }
    sys_sem_wait(semPhilo);
    int i;
    for(i=4; philos[i]!=-1;i++){
    }
    int aux = i+1;
    char * phil = "philo";
    char vec[2];
    itoa(aux, vec ,10);
    concat(phil+5, vec);
    uint64_t newphilo = sys_new_process(phil, 0, NULL, 10, FOREGROUND, philo);
    philos[i] = newphilo;
    states[i] = 0;
    char * fork = "fork";
    itoa(aux, vec ,10);
    concat(fork+4, vec);
    uint64_t * newfork = sys_sem_open(fork);
    forks[i] = newfork;
    currPhilCount++;
    sys_sem_post(semPhilo);
    return 1;
}

int removePhilo(){
    if(currPhilCount <= MIN_PHYLO_COUNT){
        return 0;
    }
    sys_sem_wait(semPhilo);
    int i;
    for(i=4; philos[i]!=-1;i++){
    }
    i--;
    sys_sem_close(forks[i]);
    forks[i] = NULL;
    sys_kill(philos[i]);
    philos[i]=-1;
    states[i]=-1;
    sys_sem_post(semPhilo);
    currPhilCount--;
    return 1;
}

int center(int pos){
 return pos;
}

int right(int pos){
  if(pos==0){
    return (currPhilCount -1);
  }
  return pos-1;
}

int left(int pos){
  if(pos == (currPhilCount -1)){
    return 0;
  }
  return pos+1;
}

void check(int pos){
  if (states[pos] == HUNGRY && states[left(pos)] != EATING && states[right(pos)] != EATING){
    sys_sem_wait(forks[center(pos)]);
    sys_sem_wait(forks[right(pos)]);
    states[pos] = EATING;
   }
}

void takeForks(int pos){
    sys_sem_wait(semPhilo);
    check(pos);
    printBoard();
    sys_sem_post(semPhilo);
}

void placeForks(int pos){
  sys_sem_wait(semPhilo);
  states[pos] = FULL;
  sys_sem_post(forks[center(pos)]);
  sys_sem_post(forks[right(pos)]);
  check(left(pos));
  check(right(pos));
  printBoard();
  if(states[left(pos)]==EATING){
    states[left(pos)]=FULL;
  }
  if(states[right(pos)]==EATING){
    states[right(pos)]=FULL;
  }
  sys_sem_post(semPhilo);
}

void philo(){
    while(start){
    sys_sem_wait(semPhilo);
    int pid = sys_get_curr_pid();
    int pos;
    int i;
    for(i=0; philos[i]!=pid;i++){}
    pos=i;
    while(states[pos]==HUNGRY){
        takeForks(pos);
        placeForks(pos);
    }
    }
     sys_sem_post(semPhilo);
}

int hungry(){
        for(int i=0; i<currPhilCount; i++){
            if(states[i]==0){
                return 1;
            }
        }
        return 0;
    }

void resetHungry(){
    for(int i=0; i<MAX_PHYLO_COUNT;i++){
        if(i<currPhilCount){
            states[i]=0;
        }
        else{
            states[i]=-1;
        }
    }
}

void printBoard(){
  int charCount = (currPhilCount * 3) + 2;
  char board[charCount];
  for (int i = 0; i < currPhilCount; i++){
    switch(states[i]){
      case HUNGRY:
        board[(3 * i)] = ' ';
        board[(3 * i) + 1] = '.';
        board[(3 * i) + 2] = ' ';
      break;
      case FULL:
        board[(3 * i)] = ' ';
        board[(3 * i) + 1] = 'T';
        board[(3 * i) + 2] = ' ';
      break;
      case EATING:
        board[(3 * i)] = ' ';
        board[(3 * i) + 1] = 'E';
        board[(3 * i) + 2] = ' ';
      break;
    }
  }
  board[charCount - 2] = '\n';
  board[charCount - 1] = 0;
  print(board);
}


void philosophers(){
    initPhilos();
    uint64_t * semPhilo = sys_sem_open("game");
    while(start){
        while(hungry() && start){
           char key = getKey();
            switch (key) {
      case 'a':
        if (!addPhilo()){
          print("Invalid Operation: Maximum philosophers count reached\n");
        } else {
          print("New philosopher added\n");
        }
      break;
      case 'r':
        if (!removePhilo()){
          print("Invalid Operation: Minimum philosophers count reached\n");
        } else {
          sys_sem_close(forks[currPhilCount]);
          if(philos[currPhilCount]!=0){
          sys_kill(philos[currPhilCount]);
          }
          print("A philosopher has been removed\n");
        }
      break;
      case 'q':
        print("\nChange the world, my final message. Goodbye\n\n");
        print("Program Finished!\n");
        for(int x = 0; x<currPhilCount;x++){
          sys_sem_close(forks[x]);
          if(philos[x]!=0){
          sys_kill(philos[x]);
          }
        }
        sys_sem_close(semPhilo);
        start = 0;
      break;
    }
        }
    print("All philosophers have eaten, resetting hunger\n");
    resetHungry();
    }
}
