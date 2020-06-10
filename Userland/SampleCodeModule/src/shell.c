#include <shell.h>
#include "test_util.h"
#include <syscalls.h>
// ----------------------------------------------------------------------------------
// Este modulo es el modulo principal de Userland
// A partir de este modulo se pueden seleccionar otros modulos a traves de la terminal
// Se hace todo el manejo de los comandos
// ----------------------------------------------------------------------------------

//Constantes para los comandos
#define INVALID_COMMAND -1
#define HELP_COMMAND 0
#define SHUTDOWN_COMMAND 1
#define INVALID_OC_COMMAND 2
#define TIME_COMMAND 3
#define BEEP_COMMAND 4
#define SLEEP_COMMAND 5
#define DATE_COMMAND 6
#define CLEAR_COMMAND 7
#define DIV_COMMAND 8
#define CREDITS_COMMAND 9
#define STARWARS_COMMAND 10
#define MARIO_COMMAND 11
#define TEST_MM 12
#define TESTPROC 13
#define PS 14
#define MEM 15
#define TESTSYNC 16
#define TESTNOSYNC 17
#define PRINTSEM 18
#define PRINTPIPES 19
#define TESTPRIO 20
#define KILL 21
#define NICE 22
#define BLOCK 23
#define WC 24
#define CAT 25
#define FILTER 26
#define LOOP 27
#define PHILOSOPHERS 28
#define PIPE 29

#define FOREGROUND 1
#define BACKGROUND 0

#define MAX_PROCESSES 60 //Should be around 80% of the the processes handled by the kernel

#define MAX_BLOCKS 128
#define MAX_MEMORY 60 * 1048576 //Should be around 80% of memory managed by the MM

enum State {ERROR, RUNNING, BLOCKED, KILLED};

typedef struct MM_rq{
  void * address;
  uint32_t size;
}mm_rq;

//Todos los comandos disponibles
const char *commands[] = {"help", "shutdown", "invalid", "time", "beep", "sleep", "date", "clear", "div", "credits", "starwars", "mario","testmm", "testproc", "ps", "mem", "testsync", "testnosync", "sem", "pipes", "testprio", "kill", "nice", "block"};

const int commandCount = 21;
int pid;
int priority;
int isBackground = 0;
int pipeCount = 0;
int pipeFd;
int fdR1 = 0;
int fdW1 = 1;
int fdR2 = 0;
int fdW2 = 1;
char firstcmd[10] = {0};
char secondcmd[10] = {0};

int getCommand(char *cmd);
void generate_invalid_opc(void);
int generate_zero_division(void);
void display_credits(void);
void make_starwars(void);
void make_mario(void);

int * sema;

//----------------------------------------------------------------------------------------------------------------------------

void endless_loop(){
  while(1);
}

uint64_t my_create_process(char * name, uint64_t entryPoint){
  return sys_new_process(name, 0, NULL, 8, BACKGROUND, entryPoint);
}

int my_kill(uint32_t pid){
  return kill(pid);
}

int my_block(uint32_t pid){
  return block(pid);
}

int my_unblock(uint32_t pid){
  return block(pid);
}



typedef struct P_rq{
  uint32_t pid;
  enum State state;
}p_rq;

void test_processes(){
  p_rq p_rqs[MAX_PROCESSES];
  uint8_t rq;
  uint8_t alive = 0;
  uint8_t action;

  while (1){

    // Create MAX_PROCESSES processes
    for(rq = 0; rq < MAX_PROCESSES; rq++){
      goToSleep(1); //lo agregamos porque al crearlos tan rapido no andaba bien
      p_rqs[rq].pid = my_create_process("endless_loop", (uint64_t)endless_loop);  // TODO: Port this call as required

      if (p_rqs[rq].pid == -1){                           // TODO: Port this as required
        printf("Error creating process\n");               // TODO: Port this as required
        return;
      }else{
        p_rqs[rq].state = RUNNING;
        alive++;
      }
    }

    // Randomly kills, blocks or unblocks processes until every one has been killed
    while (alive > 0){

      for(rq = 0; rq < MAX_PROCESSES; rq++){
        action = GetUniform(2) % 2;

        switch(action){
          case 0:
            if (p_rqs[rq].state == RUNNING || p_rqs[rq].state == BLOCKED){
              if (my_kill(p_rqs[rq].pid) == -1){          // TODO: Port this as required
                printf("Error killing process\n");        // TODO: Port this as required
                return;
              }
              p_rqs[rq].state = KILLED;
              alive--;
            }
            break;

          case 1:
            if (p_rqs[rq].state == RUNNING){
              if(my_block(p_rqs[rq].pid) == -1){          // TODO: Port this as required
                printf("Error blocking process\n");       // TODO: Port this as required
                return;
              }
              p_rqs[rq].state = BLOCKED;
            }
            break;
        }
      }

      // Randomly unblocks processes
      for(rq = 0; rq < MAX_PROCESSES; rq++)
        if (p_rqs[rq].state == BLOCKED && GetUniform(2) % 2){
          if(my_unblock(p_rqs[rq].pid) == -1){            // TODO: Port this as required
            printf("Error unblocking process\n");         // TODO: Port this as required
            return;
          }
          p_rqs[rq].state = RUNNING;
        }
    }
  }
}
//----------------------------------------------------------------------------------------------------------------------------

#define MINOR_WAIT 1000000                               // TODO: To prevent a process from flooding the screen
#define WAIT      10000000                              // TODO: Long enough to see theese processes beeing run at least twice

uint64_t my_getpid(){
  return sys_get_curr_pid();
}


uint64_t my_nice(uint64_t pid, uint64_t newPrio){
  return nice(pid, newPrio);
}


void bussy_wait(uint64_t n){
  uint64_t i;
  for (i = 0; i < n; i++);
}

void endless_loop2(){
  uint64_t pid = my_getpid();

  while(1){
    printf("%d ",pid);
    bussy_wait(MINOR_WAIT);
  }
}

#define TOTAL_PROCESSES 3

void test_prio(){
  uint64_t pids[TOTAL_PROCESSES];
  uint64_t i;

  for(i = 0; i < TOTAL_PROCESSES; i++)
    pids[i] = my_create_process("endless_loop2", endless_loop2);

  bussy_wait(WAIT);
  printf("\nCHANGING PRIORITIES...\n");

  for(i = 0; i < TOTAL_PROCESSES; i++){
    switch (i % 3){
      case 0:
        my_nice(pids[i], 1); //lowest priority
        break;
      case 1:
        my_nice(pids[i], 5); //medium priority
        break;
      case 2:
        my_nice(pids[i], 10); //highest priority
        break;
    }
  }

  bussy_wait(WAIT);
  printf("\nBLOCKING...\n");

  for(i = 0; i < TOTAL_PROCESSES; i++)
    my_block(pids[i]);

  printf("CHANGING PRIORITIES WHILE BLOCKED...\n");
  for(i = 0; i < TOTAL_PROCESSES; i++){
    switch (i % 3){
      case 0:
        my_nice(pids[i], 5); //medium priority
        break;
      case 1:
        my_nice(pids[i], 5); //medium priority
        break;
      case 2:
        my_nice(pids[i], 5); //medium priority
        break;
    }
  }

  printf("UNBLOCKING...\n");

  for(i = 0; i < TOTAL_PROCESSES; i++)
    my_unblock(pids[i]);

  bussy_wait(WAIT);
  printf("\nKILLING...\n");

  for(i = 0; i < TOTAL_PROCESSES; i++)
    my_kill(pids[i]);
}





//----------------------------------------------------------------------------------------------------------------------------



uint64_t * my_sem_open(char *sem_id, int value){
  return sys_sem_open(sem_id, value);
}

void my_sem_wait(int * sem_id){
 sys_sem_wait(sem_id);
}

void my_sem_post(int * sem_id){
  sys_sem_post(sem_id);
}

void my_sem_close(int * sem_id){
  sys_sem_close(sem_id);
}

#define N 1000
#define SEM_ID "semaforillo"
#define TOTAL_PAIR_PROCESSES 2

uint64_t global;  //shared memory

void slowInc(uint64_t *p, uint64_t inc){
  uint64_t aux = *p;
  aux += inc;
  *p = aux;
}

void my_process_inc(){
  uint64_t i;
  sema = my_sem_open(SEM_ID, 1);
  for (i = 0; i < N; i++){
    my_sem_wait(sema);
    slowInc(&global, 1);
    my_sem_post(sema);
  }

  my_sem_close(sema);
  printf("Final value: %d\n", global);

}

void my_process_dec(){
  uint64_t i;
  sema = my_sem_open(SEM_ID, 1);
  for (i = 0; i < N; i++){
    my_sem_wait(sema);
    slowInc(&global, -1);
    my_sem_post(sema);
  }
  my_sem_close(sema);
  printf("Final value: %d\n", global);

}

void test_sync(){

  uint64_t i;

  global = 0;

  printf("CREATING PROCESSES...\n");
  sema = my_sem_open(SEM_ID, 1);
  for(i = 0; i < TOTAL_PAIR_PROCESSES; i++){
    my_create_process("my_process_inc", (uint64_t)my_process_inc);
    my_create_process("my_process_dec", (uint64_t)my_process_dec);
  }

  // The last one should print 0
}

void my_process_inc_no_sem(){
  uint64_t i;
  for (i = 0; i < N; i++){
    slowInc(&global, 1);
  }

  printf("Final value: %d\n", global);
}

void my_process_dec_no_sem(){
  uint64_t i;
  for (i = 0; i < N; i++){
    slowInc(&global, -1);
  }

  printf("Final value: %d\n", global);
}

void test_no_sync(){
  uint64_t i;

  global = 0;

  printf("CREATING PROCESSES...\n");

  for(i = 0; i < TOTAL_PAIR_PROCESSES; i++){
    my_create_process("my_process_inc_no_sem", (uint64_t)my_process_inc_no_sem);
    my_create_process("my_process_dec_no_sem", (uint64_t)my_process_inc_no_sem);
  }

  // The last one should not print 0
}

//----------------------------------------------------------------------------------------------------------------------------

void test_mm(){
  mm_rq mm_rqs[MAX_BLOCKS];
  uint8_t rq;
  uint32_t total;

	while(1){
    rq = 0;
    total = 0;
    //printminus();
    // Request as many blocks as we can
    while(rq < MAX_BLOCKS && total < MAX_MEMORY){
      mm_rqs[rq].size = GetUniform(MAX_MEMORY - total - 1) + 1;
      mm_rqs[rq].address = sys_malloc(mm_rqs[rq].size); // TODO: Port this call as required

      total += mm_rqs[rq].size;
      rq++;
    }

    // Set
    uint32_t i;
    for (i = 0; i < rq; i++)
      if (mm_rqs[i].address != NULL)
        memset(mm_rqs[i].address, i, mm_rqs[i].size); // TODO: Port this call as required

    // Check
    for (i = 0; i < rq; i++)
      if (mm_rqs[i].address != NULL)
        if(!memcheck(mm_rqs[i].address, i, mm_rqs[i].size))
          printf("ERROR!\n"); // TODO: Port this call as required

    // Free
    for (i = 0; i < rq; i++)
      if (mm_rqs[i].address != NULL)
        sys_free(mm_rqs[i].address);  // TODO: Port this call as required
  }
}



void cat(){
  char buffer[1024] = {0};
  int fd = sys_get_fd(sys_get_curr_pid(), 0);
  if(fd == 0){
    scanf("%s\n", buffer);
    printf("\n");
  }else {
    sys_read(fd, buffer, 1024);
  }
  sys_write(sys_get_fd(sys_get_curr_pid(), 1), buffer, strlen(buffer));
}

void wc(){
  char buffer[1024] = {0};
  int spaces = 0;
  int fd = sys_get_fd(sys_get_curr_pid(), 0);
  if(fd == 0){
    scanf("%s\n", buffer);
    printf("\n");
  }else {
    sys_read(fd, buffer, 1024);
  }
  for (size_t i = 0; i < strlen(buffer) && buffer[i] != 0; i++) {
    if(buffer[i] == '\n'){
      spaces++;
    }
  }
  char lines[5] = {0};
  itoa(spaces, lines, 10);
  sys_write(sys_get_fd(sys_get_curr_pid(), 1), lines, strlen(lines));
}

void filter(){
  char buffer[1024] = {0};
  char auxBuff[1024] = {0};
  int fd = sys_get_fd(sys_get_curr_pid(), 0);
  if(fd == 0){
    scanf("%s\n", buffer);
    printf("\n");
  }else {
    sys_read(fd, buffer, 1024);
  }
  int j = 0;
  for (size_t i = 0; i < strlen(buffer) && buffer[i] != 0; i++) {
    if(buffer[i] != 'a' && buffer[i] != 'A' && buffer[i] != 'e' && buffer[i] != 'E' && buffer[i] != 'i' && buffer[i] != 'I' && buffer[i] != 'o' && buffer[i] != 'O' && buffer[i] != 'u' && buffer[i] != 'U'){
      auxBuff[j++] = buffer[i];
    }
  }
  sys_write(sys_get_fd(sys_get_curr_pid(), 1), auxBuff, strlen(auxBuff));
}

void loop(){
  char salute[] = "Hello my friend, if you want me to stop spamming your shell feel free to kill me. By the way my PID is: ";
  char pidd[4] = {0};
  int pidAux = sys_get_curr_pid();
  itoa(pidAux, pidd, 10);
  int i = 0;
  while(pidd[i] != 0){i++;}
  pidd[i] = '\n';
  concat(salute + strlen(salute), pidd);
  while(1){
    sys_write(sys_get_fd(sys_get_curr_pid(), 1), salute, strlen(salute));
    sleep();
  }
}



void ps(){
  sys_ps();
}

void mem(){
  sys_mem();
}

void printsem(){
  sys_print_sem();
}

int kill(int killpid){
  return sys_kill(killpid);
}

int block(int blockpid){
  return sys_block(blockpid);
}

int nice(int nicepid, int nicepriority){
  return sys_nice(nicepid, nicepriority);
}

void pipe(){

  int pidfirst;
  int pidsecond;
  if(!strcmp(firstcmd, "loop")){
    pidfirst = sys_new_process(firstcmd, 0, NULL, 10, FOREGROUND, (uint64_t)loop);
    sys_set_fd(pidfirst, 1, pipeFd);
  }
  if(!strcmp(firstcmd, "philos")){
    pidfirst = sys_new_process(firstcmd, 0, NULL, 10, FOREGROUND, (uint64_t)philosophers);
    sys_set_fd(pidfirst, 1, pipeFd);
  }
  if(!strcmp(firstcmd, "cat")){
    pidfirst = sys_new_process(firstcmd, 0, NULL, 10, FOREGROUND, (uint64_t)cat);
    sys_set_fd(pidfirst, 1, pipeFd);
  }
  if(!strcmp(firstcmd, "wc")){
    pidfirst = sys_new_process(firstcmd, 0, NULL, 10, FOREGROUND, (uint64_t)wc);
    sys_set_fd(pidfirst, 1, pipeFd);
  }
  if(!strcmp(firstcmd, "filter")){
    pidfirst = sys_new_process(firstcmd, 0, NULL, 10, FOREGROUND, (uint64_t)filter);
    sys_set_fd(pidfirst, 1, pipeFd);
  }

  if(!strcmp(secondcmd, "loop")){
    pidsecond = sys_new_process(secondcmd, 0, NULL, 10, FOREGROUND, (uint64_t)loop);
    sys_set_fd(pidsecond, 0, pipeFd);
  }
  if(!strcmp(secondcmd, "philos")){
    pidsecond = sys_new_process(secondcmd, 0, NULL, 10, FOREGROUND, (uint64_t)philosophers);
    sys_set_fd(pidsecond, 0, pipeFd);
  }
  if(!strcmp(secondcmd, "cat")){
    pidsecond = sys_new_process(secondcmd, 0, NULL, 10, FOREGROUND, (uint64_t)cat);
    sys_set_fd(pidsecond, 0, pipeFd);
  }
  if(!strcmp(secondcmd, "wc")){
    pidsecond = sys_new_process(secondcmd, 0, NULL, 10, FOREGROUND, (uint64_t)wc);
    sys_set_fd(pidsecond, 0, pipeFd);
  }
  if(!strcmp(secondcmd, "filter")){
    pidsecond = sys_new_process(secondcmd, 0, NULL, 10, FOREGROUND, (uint64_t)filter);
    sys_set_fd(pidsecond, 0, pipeFd);
  }
}



uint64_t *init_shell(void)
{
	display_welcome_message();

	print("arquiOS@ITBA: ");
	//Comando elegido
	int command = INVALID_COMMAND;
	//Buffer para el comando que se quiere escribir
	char commandBuff[MAX_BUFF_SIZE] = {0};
	//Posicion en el buffer de comando
	int commandBuffPos = 0;
	//Tecla que se toca
	char key;

	//while para la shell y su funcionamiento
	while (command != SHUTDOWN_COMMAND)
	{
		key = getKey();

		//CASO EN QUE SE PASA DE LA CANTIDAD MAXIMA DE CARACTERES
		if (commandBuffPos == MAX_BUFF_SIZE)
		{
			//Define el comando como invalido
			command = INVALID_COMMAND;
			//Imprime una linea nueva para que se vea bien
			print("\n");
			//Llama a la funcion para manejar el comando
			handle_command(command);
			//Hace un reset del buffer de comando volviendo a la posicion 0
			commandBuffPos = 0;
			//Imprime el usuario de nuevo
			print("\narquiOS@ITBA: ");
		}

		//CASO ENTER
		if (key == '\n')
		{
			//Manda el enter a la pantalla para que baje la linea y se vea bien
			writeKey(&key);
			//Agrega terminacion en 0 al buffer de comando
			commandBuff[commandBuffPos] = 0;
			//Recupera el comando que fue elegido
			command = getCommand(commandBuff);

      // if(secondcmd[0] != 0){
      //
      // }
			//Llama a la funcion que decide como actuar en frente del comando
			handle_command(command);
			//Hace un reset del buffer de comando volviendo a la posicion 0
			commandBuffPos = 0;

			//Vuelve a imprimir el usuario para que se vea bien
			if (command != SHUTDOWN_COMMAND)
				print("\narquiOS@ITBA: ");
		}
		//CASO BACKSPACE - DELETE
		else if (key == '\b')
		{
			//En el caso de que se borro todo, para que no borre de mas
			if (commandBuffPos != 0)
			{
				//Manda el backspace para que borre fisicamente a la tecla
				writeKey(&key);
				//Borra la letra del buffer de comando
				commandBuffPos--;
			}
		}
		//CASO CUALQUIER OTRA TECLA
		else if (key != 0)
		{
			//Manda la letra a pantalla
			writeKey(&key);
			//Manda la letra al buffer de comando
			commandBuff[commandBuffPos] = key;
			//Incrementa la longitud de la palabra
			commandBuffPos++;
		}
	}

	display_goodbye_message();

	return (uint64_t *)RETURN_ADRESS;
}

int getCommand(char *cmd)
{
	//Itero el array de comandos para ver cual es el que se elige
	int result = INVALID_COMMAND;

	for (int i = 0; i < commandCount && result == INVALID_COMMAND; i++)
	{
		//En el caso de que sean iguales
		if (!strcmp(cmd, commands[i]))
		{
			return i;
		}
	}

  if(!strncmp(cmd, "kill ", 4)){
    char * cmdaux = cmd+5;
    int j = 0;
    while(*cmdaux != 0){
      if(!isNumeric(*cmdaux)){
        return result;
      }
      cmdaux += 1;
      j++;
    }
    pid = atoi(cmd + 5,j);
    if(pid == -1){
      return result;
    }
    return KILL;
  }

  if(!strncmp(cmd, "block ", 5)){
    char * cmdaux2 = cmd+6;
    int a = 0;

    while(*cmdaux2 != 0){
      if(!isNumeric(*cmdaux2)){
        return result;
      }
      cmdaux2 += 1;
      a++;
    }
    pid = atoi(cmd + 6,a);
    if(pid == -1){
      return result;
    }
    return BLOCK;
  }

  if(!strncmp(cmd, "nice ", 4)){
    char * cmdaux3 = cmd+5;
    int b = 0;
    while(*cmdaux3 != ' '){
      if(!isNumeric(*cmdaux3)){
        return result;
      }
      cmdaux3 += 1;
      b++;
    }
    pid = atoi(cmd + 5,b);
    if(pid == -1){
      return result;
    }
    int x = 0;
    cmdaux3 += 1;
    while(*cmdaux3 != 0){
      if(!isNumeric(*cmdaux3)){
        return result;
      }
      cmdaux3 += 1;
      x++;
    }
    priority = atoi(cmd + 5 + b + 1, x);
    if(priority == -1){
      return result;
    }
    return NICE;
  }

  // char *cmdAux = cmd;
  // int count = 0;
  int f = 0;
  int s = 0;
  int spaceflag = 0;
  int pipeflag = 0;
  fdR1 = 0;
  fdW1 = 1;
  fdR2 = 0;
  fdW2 = 1;
  for(int q = 0; q < 10; q++){
    firstcmd[q] = 0;
    secondcmd[q] = 0;
  }
  while(*cmd != 0){
    if(!spaceflag && isAlpha(*cmd)){
      firstcmd[f++] = *cmd;
    }else if(spaceflag && pipeflag && isAlpha(*cmd)){
      secondcmd[s++] = *cmd;
    }

    if(*cmd == ' ' && !spaceflag){
      spaceflag = 1;
    }

    if(*cmd == '|' && spaceflag){
      if(*(cmd+1) != ' '){
        return result;
      }
      pipeflag = 1;
    }

    if(*cmd == '&' && !spaceflag && !pipeflag){
      if(*(cmd-1) == ' ' || *(cmd+1) != 0){
        return result;
      }
      if(!strcmp(firstcmd, "loop")){
        isBackground = 1;
        return LOOP;
      }
      if(!strcmp(firstcmd, "philos")){
        isBackground = 1;
        return PHILOSOPHERS;
      }
      if(!strcmp(firstcmd, "cat")){
        isBackground = 1;
        return CAT;
      }
      if(!strcmp(firstcmd, "wc")){
        isBackground = 1;
        return WC;
      }
      if(!strcmp(firstcmd, "filter")){
        isBackground = 1;
        return FILTER;
      }
      return result;
    }


    cmd++;
  }


  if(!strcmp(firstcmd, "cat") && !spaceflag && !pipeflag){
    isBackground = 0;
    return CAT;
  }
  if(!strcmp(firstcmd, "wc") && !spaceflag && !pipeflag){
    isBackground = 0;
    return WC;
  }
  if(!strcmp(firstcmd, "filter") && !spaceflag && !pipeflag){
    isBackground = 0;
    return FILTER;
  }
  if(!strcmp(firstcmd, "philos") && !spaceflag && !pipeflag){
    isBackground = 0;
    return PHILOSOPHERS;
  }
  if(!strcmp(firstcmd, "loop") && !spaceflag && !pipeflag){
    isBackground = 0;
    return LOOP;
  }

  if(pipeflag && spaceflag){
    char pipeName[10];
    strcpy(pipeName, "pipe");
    char pipeIndex[4];
    itoa(pipeCount, pipeIndex, 10);
    concat((char *)(pipeName + 4), pipeIndex);
    pipeFd = sys_open_pipe(pipeName);
    pipeCount++;
    isBackground = 0;
    fdW1 = pipeFd;
    fdR1 = 0;
    fdW2 = 1;
    fdR2 = pipeFd;
    return PIPE;
  }

	return result;
}

//Switch para el comando elegido
//Recibe el comando como un parametro
void handle_command(int cmd)
{
	switch (cmd)
	{
	case HELP_COMMAND:
		display_help();
		break;
	//Retorna y sale del while, y no se puede hacer nada mas
	case SHUTDOWN_COMMAND:
		clearScreen();
		display_goodbye_message();
		sys_shutdown();
		break;
	case INVALID_OC_COMMAND:
		generate_invalid_opc();
		break;
	case DIV_COMMAND:
		generate_zero_division();
		break;
	case DATE_COMMAND:
		display_date();
		break;
	case TIME_COMMAND:
		display_time();
		break;
	case BEEP_COMMAND:
		make_sound();
		break;
	case SLEEP_COMMAND:
		sleep();
		break;
	case CLEAR_COMMAND:
		clearScreen();
		break;
	case INVALID_COMMAND:
		display_invalid_command();
		break;
	case CREDITS_COMMAND:
		display_credits();
		break;
	case STARWARS_COMMAND:
		make_starwars();
		break;
	case MARIO_COMMAND:
		make_mario();
		break;
  case PS:
    ps();
    break;
  case MEM:
    mem();
    break;
  case TEST_MM:
    test_mm();
    break;
  case TESTSYNC:
    test_sync();
    break;
  case TESTNOSYNC:
    test_no_sync();
    break;
  case TESTPROC:
    sys_new_process("testproc",0,NULL,10,FOREGROUND,(uint64_t)test_processes);
    break;
  case PRINTSEM:
    printsem();
    break;
  case TESTPRIO:
    test_prio();
    break;
  case KILL:
    kill(pid);
    break;
  case NICE:
    nice(pid, priority);
    break;
  case BLOCK:
    block(pid);
    break;
  case LOOP:
    sys_new_process("loop", 0, NULL, 10, isBackground ? BACKGROUND : FOREGROUND, (uint64_t)loop);
    break;
  case CAT:
    sys_new_process("cat", 0, NULL, 10, isBackground ? BACKGROUND : FOREGROUND, (uint64_t)cat);
    break;
  case WC:
    sys_new_process("wc", 0, NULL, 10, isBackground ? BACKGROUND : FOREGROUND, (uint64_t)wc);
    break;
  case PHILOSOPHERS:
    sys_new_process("philos", 0, NULL, 10, isBackground ? BACKGROUND : FOREGROUND, (uint64_t)philosophers);
    break;
  case FILTER:
    sys_new_process("filter", 0, NULL, 10, isBackground ? BACKGROUND : FOREGROUND, (uint64_t)filter);
    break;
  case PIPE:
    pipe();
    break;
  case PRINTPIPES:
    sys_print_pipes();
    break;
	}
  printf("\n");
}

/*
	Imprime el mensaje de bienvenida
	Se generaron con una pagina web
*/
void display_welcome_message(void)
{
	clearScreen();
	print("						                                               /$$  /$$$$$$   /$$$$$$\n");
	print("						                                              |__/ /$$__  $$ /$$__  $$\n");
	print("						        /$$$$$$   /$$$$$$   /$$$$$$  /$$   /$$ /$$| $$  \\ $$| $$  \\__/\n");
	print("						       |____  $$ /$$__  $$ /$$__  $$| $$  | $$| $$| $$  | $$|  $$$$$$ \n");
	print("						        /$$$$$$$| $$  \\__/| $$  \\ $$| $$  | $$| $$| $$  | $$ \\____  $$\n");
	print("						       /$$__  $$| $$      | $$  | $$| $$  | $$| $$| $$  | $$ /$$  \\ $$\n");
	print("						      |  $$$$$$$| $$      |  $$$$$$$|  $$$$$$/| $$|  $$$$$$/|  $$$$$$/\n");
	print("						       \\_______/|__/       \\____  $$ \\______/ |__/ \\______/  \\______/ \n");
	print("						                                | $$                                  \n");
	print("						                                | $$    \n");
	print("						                                |__/   \n\n");
	print("													Trabajo Practico Especial\n");
	print("								Arquitectura de Computadoras\t|\tSistemas Operativos\n");
	print("									 1er Cuatrimestre - 2019\t|\t1er Cuatrimestre - 2020\n\n");
	print("													  Welcome to arquiOS\n");
	print("										Type \"help\" to discover all available commands\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
}

void display_help(void)
{
	print("help - Displays available commands and their usage\n");
	print("shutdown - Shuts down the system\n");
	print("time - Displays current system time\n");
	print("date - Displays current system date\n");
	print("beep - Makes the system go Beep!\n");
	print("sleep - Makes the system sleep for 5 seconds\n");
	print("div - Performs a division by zero\n");
	print("invalid - Executes an invalid operation\n");
	print("clear - Clears the screen\n");
	print("credits - Displays info about the group\n");
	print("starwars - Makes a cool Star Wars sound!\n");
	print("mario - Makes a cool Mario sound!\n");
	print("testmm - Tests memory manager\n");
  print("testproc - Tests processes\n");
  print("ps - Prints basic information abouth each process\n");
  print("mem - Prints memory state\n");
  print("sem - Prints semaphores\n");
  print("testsync - Tests semaphore sync\n");
  print("testnosync - Tests semaphora no sync\n");
  print("testnosync - Tests processes priority\n");
  print("pipes - Prints pipes and their states\n");
  print("cat - Prints stdin\n");
  print("loop - Prints it's own pid with a message\n");
  print("wc - Counts new lines in stdin\n");
  print("filter - Filters stdin vocals\n");
  print("philos - Run philosophers game\n");
  print("kill - Kill a process given it's pid\n");
  print("nice - Changes a process' priority given it's pid and new priority\n");
  print("block - Blocks a process given it's pid\n");
}

void display_time(void)
{
	print("The time is ");
	char time[20];
	getTime(time);
	print(time);
}

void display_date(void)
{
	print("The date is ");
	char date[20];
	getDate(date);
	print(date);
}

void display_credits(void)
{
	print("The authors of this OS are:\n");
	print("Arquitectura de Computadoras:     Nacho Villanueva - Ignacio Ribas   - Gonzalo Hirsch\n");
	print("Sistemas Operativos:              Luca La Mattina - Rodrigo Fera\n");
}

void generate_invalid_opc()
{
	uint64_t invalid = 0xFFFFFFFFFFFF;
	uint64_t *ptr = &invalid;
	((void (*)())ptr)();
}

int generate_zero_division()
{
	int a = 10;
	int b = 0;
	return a / b;
}

void make_sound(void)
{
	makeSound(800, 5);
}

void make_starwars(void)
{
	makeSound(440, 500 / 3);
	makeSound(440, 1500 / 3);
	makeSound(440, 1500 / 3);
	makeSound(349, 1050 / 3);
	makeSound(523, 450 / 3);
	makeSound(440, 1500 / 3);
	makeSound(349, 1050 / 3);
	makeSound(523, 450 / 3);
	makeSound(440, 1000 / 3);
	goToSleep(10); // Delay 250 milliseconds !!!!
	makeSound(659, 1500 / 3);
	makeSound(659, 1500 / 3);
	makeSound(659, 1500 / 3);
	makeSound(698, 1050 / 3);
	makeSound(523, 450 / 3);
	makeSound(415, 1500 / 3);
	makeSound(349, 1050 / 3);
	makeSound(523, 450 / 3);
	makeSound(440, 1000 / 3);
	goToSleep(10); // Delay 250 milliseconds !!!!
	makeSound(880, 1500 / 3);
	makeSound(440, 1050 / 3);
	makeSound(440, 450 / 3);
	makeSound(880, 1500 / 3);
	makeSound(830, 750 / 3);
	makeSound(784, 750 / 3);
	makeSound(740, 375 / 3);
	makeSound(698, 375 / 3);
	makeSound(740, 750 / 3);
	goToSleep(7); // Delay 250 milliseconds !!!!
	makeSound(455, 750 / 3);
	makeSound(622, 1500 / 3);
	makeSound(587, 750 / 3);
	makeSound(554, 750 / 3);
	makeSound(523, 375 / 3);
	makeSound(466, 375 / 3);
	makeSound(523, 750 / 3);
	goToSleep(7); // Delay 250 milliseconds !!!!
	makeSound(349, 375 / 3);
	makeSound(415, 1500 / 3);
	makeSound(349, 375 / 3);
	makeSound(440, 375 / 3);
	makeSound(523, 1500 / 3);
	makeSound(440, 375 / 3);
	makeSound(523, 375 / 3);
	makeSound(659, 1000 / 3);
	makeSound(880, 1500 / 3);
	makeSound(440, 1050 / 3);
	makeSound(440, 450 / 3);
	makeSound(880, 1500 / 3);
	makeSound(830, 750 / 3);
	makeSound(784, 750 / 3);
	makeSound(740, 375 / 3);
	makeSound(698, 375 / 3);
	makeSound(740, 750 / 3);
	goToSleep(7);
	makeSound(455, 750 / 3);
	makeSound(622, 1500 / 3);
	makeSound(587, 750 / 3);
	makeSound(554, 750 / 3);
	makeSound(523, 375 / 3);
	makeSound(466, 375 / 3);
	makeSound(523, 750 / 3);
	goToSleep(7);
	makeSound(349, 750 / 3);
	makeSound(415, 1500 / 3);
	makeSound(349, 375 / 3);
	makeSound(523, 375 / 3);
	makeSound(440, 1500 / 3);
	makeSound(349, 375 / 3);
	makeSound(261, 375 / 3);
	makeSound(440, 1000 / 3);
	goToSleep(7);
}

void make_mario(void)
{
	makeSound(659, 125 / 35);
	makeSound(659, 125 / 35);
	goToSleep(125 / 35);
	makeSound(659, 125 / 35);
	goToSleep(167 / 35);
	makeSound(523, 125 / 35);
	makeSound(659, 125 / 35);
	goToSleep(125 / 35);
	makeSound(784, 125 / 35);
	goToSleep(375 / 35);
	makeSound(392, 125 / 35);
	goToSleep(375 / 35);
	makeSound(523, 125 / 35);
	goToSleep(250 / 35);
	makeSound(392, 125 / 35);
	goToSleep(250 / 35);
	makeSound(330, 125 / 35);
	goToSleep(250 / 35);
	makeSound(440, 125 / 35);
	goToSleep(125 / 35);
	makeSound(494, 125 / 35);
	goToSleep(125 / 35);
	makeSound(466, 125 / 35);
	goToSleep(42 / 35);
	makeSound(440, 125 / 35);
	goToSleep(125 / 35);
	makeSound(392, 125 / 35);
	goToSleep(125 / 35);
	makeSound(659, 125 / 35);
	goToSleep(125 / 35);
	makeSound(784, 125 / 35);
	goToSleep(125 / 35);
	makeSound(880, 125 / 35);
	goToSleep(125 / 35);
	makeSound(698, 125 / 35);
	makeSound(784, 125 / 35);
	goToSleep(125 / 35);
	makeSound(659, 125 / 35);
	goToSleep(125 / 35);
	makeSound(523, 125 / 35);
	goToSleep(125 / 35);
	makeSound(587, 125 / 35);
	makeSound(494, 125 / 35);
	goToSleep(125 / 35);
	makeSound(523, 125 / 35);
	goToSleep(250 / 35);
	makeSound(392, 125 / 35);
	goToSleep(250 / 35);
	makeSound(330, 125 / 35);
	goToSleep(250 / 35);
	makeSound(440, 125 / 35);
	goToSleep(125 / 35);
	makeSound(494, 125 / 35);
	goToSleep(125 / 35);
	makeSound(466, 125 / 35);
	goToSleep(42 / 35);
	makeSound(440, 125 / 35);
	goToSleep(125 / 35);
	makeSound(392, 125 / 35);
	goToSleep(125 / 35);
	makeSound(659, 125 / 35);
	goToSleep(125 / 35);
	makeSound(784, 125 / 35);
	goToSleep(125 / 35);
	makeSound(880, 125 / 35);
	goToSleep(125 / 35);
	makeSound(698, 125 / 35);
	makeSound(784, 125 / 35);
	goToSleep(125 / 35);
	makeSound(659, 125 / 35);
	goToSleep(125 / 35);
	makeSound(523, 125 / 35);
	goToSleep(125 / 35);
	makeSound(587, 125 / 35);
	makeSound(494, 125 / 35);
	goToSleep(375 / 35);
	makeSound(784, 125 / 35);
	makeSound(740, 125 / 35);
	makeSound(698, 125 / 35);
	goToSleep(42 / 35);
	makeSound(622, 125 / 35);
	goToSleep(125 / 35);
	makeSound(659, 125 / 35);
	goToSleep(167 / 35);
	makeSound(415, 125 / 35);
	makeSound(440, 125 / 35);
	makeSound(523, 125 / 35);
	goToSleep(125 / 35);
	makeSound(440, 125 / 35);
	makeSound(523, 125 / 35);
	makeSound(587, 125 / 35);
	goToSleep(250 / 35);
	makeSound(784, 125 / 35);
	makeSound(740, 125 / 35);
	makeSound(698, 125 / 35);
	goToSleep(42 / 35);
	makeSound(622, 125 / 35);
	goToSleep(125 / 35);
	makeSound(659, 125 / 35);
	goToSleep(167 / 35);
	makeSound(698, 125 / 35);
	goToSleep(125 / 35);
	makeSound(698, 125 / 35);
	makeSound(698, 125 / 35);
	goToSleep(625 / 35);
	makeSound(784, 125 / 35);
	makeSound(740, 125 / 35);
	makeSound(698, 125 / 35);
	goToSleep(42 / 35);
	makeSound(622, 125 / 35);
	goToSleep(125 / 35);
	makeSound(659, 125 / 35);
	goToSleep(167 / 35);
	makeSound(415, 125 / 35);
	makeSound(440, 125 / 35);
	makeSound(523, 125 / 35);
	goToSleep(125 / 35);
	makeSound(440, 125 / 35);
	makeSound(523, 125 / 35);
	makeSound(587, 125 / 35);
	goToSleep(250 / 35);
	makeSound(622, 125 / 35);
	goToSleep(250 / 35);
	makeSound(587, 125 / 35);
	goToSleep(250 / 35);
	makeSound(523, 125 / 35);
	goToSleep(1125 / 35);
	makeSound(784, 125 / 35);
	makeSound(740, 125 / 35);
	makeSound(698, 125 / 35);
	goToSleep(42 / 35);
	makeSound(622, 125 / 35);
	goToSleep(125 / 35);
	makeSound(659, 125 / 35);
	goToSleep(167 / 35);
	makeSound(415, 125 / 35);
	makeSound(440, 125 / 35);
	makeSound(523, 125 / 35);
	goToSleep(125 / 35);
	makeSound(440, 125 / 35);
	makeSound(523, 125 / 35);
	makeSound(587, 125 / 35);
	goToSleep(250 / 35);
	makeSound(784, 125 / 35);
	makeSound(740, 125 / 35);
	makeSound(698, 125 / 35);
	goToSleep(42 / 35);
	makeSound(622, 125 / 35);
	goToSleep(125 / 35);
	makeSound(659, 125 / 35);
	goToSleep(167 / 35);
	makeSound(698, 125 / 35);
	goToSleep(125 / 35);
	makeSound(698, 125 / 35);
	makeSound(698, 125 / 35);
	goToSleep(625 / 35);
	makeSound(784, 125 / 35);
	makeSound(740, 125 / 35);
	makeSound(698, 125 / 35);
	goToSleep(42 / 35);
	makeSound(622, 125 / 35);
	goToSleep(125 / 35);
	makeSound(659, 125 / 35);
	goToSleep(167 / 35);
	makeSound(415, 125 / 35);
	makeSound(440, 125 / 35);
	makeSound(523, 125 / 35);
	goToSleep(125 / 35);
	makeSound(440, 125 / 35);
	makeSound(523, 125 / 35);
	makeSound(587, 125 / 35);
	goToSleep(250 / 35);
	makeSound(622, 125 / 35);
	goToSleep(250 / 35);
	makeSound(587, 125 / 35);
	goToSleep(250 / 35);
	makeSound(523, 125 / 35);
}

void sleep(void)
{
	goToSleep(50);
}

void display_invalid_command(void)
{
	print("Invalid command, type \'help\' to view system commands\n");
}



void display_goodbye_message(void)
{
	clearScreen();
	print("							  /$$$$$$                            /$$ /$$                          \n");
	print("							 /$$__  $$                          | $$| $$                          \n");
	print("							| $$  \\__/  /$$$$$$   /$$$$$$   /$$$$$$$| $$$$$$$  /$$   /$$  /$$$$$$ \n");
	print("							| $$ /$$$$ /$$__  $$ /$$__  $$ /$$__  $$| $$__  $$| $$  | $$ /$$__  $$\n");
	print("							| $$|_  $$| $$  \\ $$| $$  \\ $$| $$  | $$| $$  \\ $$| $$  | $$| $$$$$$$$\n");
	print("							| $$  \\ $$| $$  | $$| $$  | $$| $$  | $$| $$  | $$| $$  | $$| $$_____/\n");
	print("							|  $$$$$$/|  $$$$$$/|  $$$$$$/|  $$$$$$$| $$$$$$$/|  $$$$$$$|  $$$$$$$\n");
	print("							 \\______/  \\______/  \\______/  \\_______/|_______/  \\____  $$ \\_______/\n");
	print("							                                                   /$$  | $$          \n");
	print("							                                                  |  $$$$$$/          \n");
	print("					    		                                               \\______/           \n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
}
