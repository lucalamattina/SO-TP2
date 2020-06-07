#ifndef SYSCALLS_H
#define SYSCALLS_H

//Incluye a interrupts para poder hacer la llamada a la int 80
#include <interrupts.h>
#include <stdint.h>
#include <stddef.h>
#include <utils.h>

#define READ 1
#define WRITE 2
#define BEEP 3
#define TICKS 4
#define TIME 5
#define SLEEP 6
#define DATE 7
#define CLEAR 8
#define OVER_CLOCK 10
#define SHUTDOWN 11
#define MALLOC 12
#define FREE 13
#define NEWPROC 14
#define PS 15
#define KILL 16
#define NICE 17
#define BLOCK 18
#define MEM 19
#define SEMOPEN 20
#define SEMPOST 21
#define SEMWAIT 22
#define SEMCLOSE 23
#define PRINTSEM 24

void sys_write(int fd, const char *buf, int count);

void sys_write_key(int fd, const char * buf);

void sys_get_key(int fd, char *buf);

void sys_over_clock(int rate);

void sys_read(int fd, char *buf, int count);

void sys_sleep(int ticks);

void sys_beep(int freq, int time);

uint64_t sys_get_ticks(void);

void sys_clear_console(void);

int sys_time(int selector);

void sys_shutdown();

void * sys_malloc(size_t size);

void sys_free(void * p);

int sys_new_process(char * name, int argc, char ** argv, int priority, int isForeground, int (*entryPoint) (int, char **));

void sys_ps();

void sys_kill(int pid);

void sys_nice(int pid, int priority);

void sys_block(int pid);

void sys_mem();

int * sys_sem_open(char * name);

void sys_sem_post(int * sema);

void sys_sem_wait(int * sema);

void sys_sem_close(int * sema);

void sys_print_sem();

#endif
