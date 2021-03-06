#include <syscalls.h>

// ----------------------------------------------------------------------------------
// Este modulo es el modulo de Syscalls
// Llamadas a la int 80
// Aca se agregan todas las llamadas que va a usar Userland,
// para que el usuario no tenga acceso directo
// desde el shell a estas llamadas
// ----------------------------------------------------------------------------------

void sys_write(int fd, const char *buf, int count){
	_int80((uint64_t)WRITE, (uint64_t)fd, (uint64_t)buf, (uint64_t)count, 0, 0);
}

void sys_read(int fd, char *buf, int count){
	_int80((uint64_t)READ, (uint64_t)fd, (uint64_t)buf, (uint64_t)count, 0, 0);
}

void sys_get_key(int fd, char * buf){
	_int80((uint64_t)READ, (uint64_t)fd, (uint64_t)buf, (uint64_t)1, 0, 0);
}

void sys_write_key(int fd, const char * buf){
	_int80((uint64_t)WRITE, (uint64_t)fd, (uint64_t)buf, (uint64_t)1, 0, 0);
}

void sys_beep(int freq, int time){
	_int80((uint64_t)BEEP, (uint64_t)freq, (uint64_t)time, 0, 0, 0);
}

int sys_time(int selector){
	uint64_t time = _int80((uint64_t)TIME, (uint64_t)selector, 0, 0, 0, 0);
	return time;
}

void sys_over_clock(int rate){
	_int80((uint64_t)OVER_CLOCK, (uint64_t)rate, 0, 0, 0, 0);
}

void sys_sleep(int ticks){
	_int80((uint64_t)SLEEP, 0, 0, (uint64_t)ticks, 0, 0);
}

void sys_clear_console(void){
	_int80((uint64_t)CLEAR, 0, 0, 0, 0, 0);
}

uint64_t sys_get_ticks(void){
	int ticks;
	_int80((uint64_t)TICKS, 0, (uint64_t)&ticks, 0, 0, 0);
	return ticks;
}

void sys_shutdown(){
    _int80(SHUTDOWN, 0, 0, 0, 0, 0);
}

void * sys_malloc(size_t size){
	return (void *)_int80((uint64_t)MALLOC,(uint64_t)size,0,0,0,0);
}

void sys_free(void * p){
	_int80((uint64_t)FREE, (uint64_t)p,0,0,0,0);
}

int sys_new_process(char * name, int argc, char ** argv, int priority, int isForeground, uint64_t entryPoint){
	int priorityAux = isForeground == 0 ? (-1) * priority : priority;
	return (int)_int80((uint64_t)NEWPROC,(uint64_t) name, (uint64_t)argc, (uint64_t)argv, (uint64_t)priorityAux, (uint64_t)entryPoint);
}

void sys_ps(void){
	_int80((uint64_t)PSS, 0, 0, 0, 0, 0);
}

int sys_kill(int pid){
	return (int)_int80((uint64_t)KILLS, (uint64_t)pid, 0, 0, 0, 0);
}

int sys_nice(int pid, int priority){
	return (int)_int80((uint64_t)NICES, (uint64_t)pid, (uint64_t)priority, 0, 0, 0);
}

int sys_block(int pid){
	return (int)_int80((uint64_t)BLOCKS, (uint64_t)pid, 0, 0, 0, 0);
}

void sys_mem(){
	_int80((uint64_t)MEMM, 0, 0, 0, 0, 0);
}

int * sys_sem_open(char * name, int value){
	return (int *)_int80((uint64_t)SEMOPEN, (uint64_t)name, (uint64_t)value, 0, 0, 0);
}

void sys_sem_post(int * sema){
	_int80((uint64_t)SEMPOST, (uint64_t)sema, 0, 0, 0, 0);
}

void sys_sem_wait(int * sema){
	_int80((uint64_t)SEMWAIT, (uint64_t)sema, 0, 0, 0, 0);
}

void sys_sem_close(int * sema){
	_int80((uint64_t)SEMCLOSE, (uint64_t)sema, 0, 0, 0, 0);
}

int sys_get_curr_pid(){
	return _int80((uint64_t)GETCURRPID, 0, 0, 0, 0, 0);
}
void sys_print_sem(){
	_int80((uint64_t)PRINTSEMM, 0, 0, 0, 0, 0);
}

int sys_open_pipe(char * name){
	return (int)_int80((uint64_t)OPENPIPE, (uint64_t)name, 0, 0, 0, 0);
}

void sys_print_pipes(){
	_int80((uint64_t)PRINTPIPESS, 0, 0, 0, 0, 0);
}

void sys_set_fd(int pid, int fdToModify, int newFd){
	_int80((uint64_t)SETFD, (uint64_t)pid, (uint64_t)fdToModify, (uint64_t)newFd, 0, 0);
}

int sys_get_fd(int pid, int fdPos){
	return (int)_int80((uint64_t)GETFD, (uint64_t)pid, (uint64_t)fdPos, 0, 0, 0);
}

int sys_get_visibility(int pid){
	return (int)_int80((uint64_t)GETVISIBILITY, (uint64_t)pid, 0, 0, 0, 0);
}
