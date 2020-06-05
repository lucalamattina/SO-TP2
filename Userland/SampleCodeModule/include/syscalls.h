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
#define OPEN 16
#define POST 17
#define WAIT 18
#define KILL 19
#define NICE 20
#define BLOCK 21
#define MEM 22

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

int sys_new_process(char * name, int argc, char ** argv, int priority, int (*entryPoint) (int, char **));

void sys_ps();

void sys_kill(int pid);

void sys_nice(int pid, int priority);

void sys_block(int pid);

void sys_mem();

sem * sys_semOpen(char * name);

void sys_semPost(sem * sema);

void sys_semWait(sem * sema);



#endif
