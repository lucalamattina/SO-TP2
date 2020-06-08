#include <syscall.h>
#include <naiveConsole.h>
#include <stdint.h>
#include <console.h>
#include <scheduler.h>
#include <semaphore.h>

extern void hang();
extern void over_clock(int rate);

void handle_sys_write(int fd, const char * buf, int length);

void handle_sys_read(int fd, char * buf, int length);

void handle_sys_beep(int freq, int time);

void handle_sys_get_ticks(int * ticks);

void handle_sys_sleep(int ticks);

void handle_sys_over_clock(int rate);

int handle_sys_time(uint64_t selector);

void handle_sys_clear_console(void);

void handle_sys_draw_pixel(int x, int y, int r, int g, int b);

void * handle_pmalloc(size_t size);

void handle_pfree(void * p);

int handle_sys_new_process(char * name, int argc, char ** argv, int priority, int (*entryPoint) (int, char **));

void handle_sys_ps();

void handle_sys_kill(int pid);

void handle_sys_block(int pid);

void handle_sys_nice(int pid, int priority);

void handle_sys_mem();

int * handle_sys_sem_open(char * name);

void handle_sys_sem_post(int * sema);

void handle_sys_sem_wait(int * sema);

void handle_sys_sem_close(int * sema);

int handle_sys_get_curr_pid();
void hande_sys_print_sem();

//Handler de la llamada a la int 80
uint64_t handleSyscall(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9){
    switch(rdi){
		case WRITE:
			handle_sys_write(rsi, (char *)rdx, rcx);
		break;
		case READ:
			handle_sys_read(rsi, (char *)rdx, rcx);
		break;
		case TIME:
			return handle_sys_time(rsi);
		break;
		case BEEP:
			handle_sys_beep(rsi, rdx);
		break;
		case SLEEP:
			handle_sys_sleep(rcx);
		break;
		case OVER_CLOCK:
			handle_sys_over_clock(rsi);
		break;
		case CLEAR:
			handle_sys_clear_console();
		break;
		case TICKS:
			handle_sys_get_ticks((int *)rdx);
		break;
	    case SHUTDOWN:
	        //Metodo facil: Halteo la PC, como seria en la version original
	        //del kernel. Se podria buscar una forma de apagar la PC
	        //realmente, es decir, apagar el hardware.
            hang();
        break;
      case MALLOC:
        return (uint64_t)handle_pmalloc(rsi);
        break;
      case FREE:
        handle_pfree(rsi);
        break;
      case NEWPROC:
        return handle_sys_new_process(rsi, rdx, rcx, r8, r9);
        break;
      case PS:
        handle_sys_ps();
        break;
      case KILL:
        handle_sys_kill(rsi);
        break;
      case NICE:
        handle_sys_nice(rsi, rdx);
        break;
      case BLOCK:
        handle_sys_block(rsi);
        break;
      case MEM:
        handle_sys_mem();
        break;
      case SEMOPEN:
        return (uint64_t)handle_sys_sem_open(rsi);
        break;
      case SEMPOST:
        handle_sys_sem_post(rsi);
        break;
      case SEMWAIT:
        handle_sys_sem_wait(rsi);
        break;
      case SEMCLOSE:
        handle_sys_sem_close(rsi);
        break;
      case PRINTSEM:
        handle_sys_print_sem();
        break;
      case GETCURRPID:
        return handle_sys_get_curr_pid();
        break;

	}
	return 0;
}

//Handler para la system write
void handle_sys_write(int fd, const char * buf, int length){
    print_N(buf, length);
}

//Handler para la system get ticks
void handle_sys_get_ticks(int * ticks){
	*(ticks) = ticks_elapsed();
}

//Handler para la system clear console
void handle_sys_clear_console(void){
	clear_console();
}


//Handler para la system read
//El fd es el File Descriptor, no lo utilizamos porque no es necesario en nuestro caso
//Esta para que se pueda implementar en el futuro
void handle_sys_read(int fd, char * buf, int length){
  if(fd == 0){
  	for (int i = 0; i < length; i++){
  		*(buf + i) = getChar();
  	}
  }
}

//Handler para la system SLEEP
//Esta hecho en ticks, para tener mas control del tiempo
void handle_sys_sleep(int ticks){
	timer_wait(ticks);
}

//Handler para la system Beep
//Recibe la frecuencia y el tiempo en ticks que se hace el sonido
void handle_sys_beep(int freq, int time){
	beep(freq, time);
}

//Handler para la system overclock
//Esta deprecada, no se utiliz
//La dejamos para que se haga uso si se necesita, reprograma el PIT
void handle_sys_over_clock(int rate){
	over_clock(rate);
}

//Handler para la system time
//Recibe un selector que se utiliza para saber que registro acceder
int handle_sys_time(uint64_t selector){
	return get_time(selector);
}

void * handle_pmalloc(size_t size){
  return pmalloc(size);
}

void handle_pfree(void * p){
  pfree(p);
}

int handle_sys_new_process(char * name, int argc, char ** argv, int priority, int (*entryPoint) (int, char **)){
  process * newp = newProcess(name, argc, argv, priority, entryPoint);
  int ret = newp->pid;
  fakeStack(newp);
  return ret;
}

void handle_sys_ps(void){
  ps();
}

void handle_sys_kill(int pid){
  kill(pid);
}

void handle_sys_nice(int pid, int priority){
  nice(pid, priority);
}

void handle_sys_block(int pid){
  block(pid);
}

void handle_sys_mem(){
  mem();
}

int * handle_sys_sem_open(char * name){
  return semOpen(name);
}

void handle_sys_sem_post(int * sema){
  semPost(sema);
}

void handle_sys_sem_wait(int * sema){
  semWait(sema);
}

void handle_sys_sem_close(int * sema){
  semClose(sema);
}

void handle_sys_print_sem(){
  printsem();
}

int handle_sys_get_curr_pid(){
  return getCurrentPid();
}