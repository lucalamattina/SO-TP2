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
#define PSS 15
#define KILLS 16
#define NICES 17
#define BLOCKS 18
#define MEMM 19
#define SEMOPEN 20
#define SEMPOST 21
#define SEMWAIT 22
#define SEMCLOSE 23
#define PRINTSEMM 24
#define OPENPIPE 25
#define PRINTPIPESS 26
#define SETFD 27
#define GETFD 28
#define GETCURRPID 29
#define GETVISIBILITY 30

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

int sys_new_process(char * name, int argc, char ** argv, int priority, int isForeground, uint64_t entryPoint);

void sys_ps();

int sys_kill(int pid);

int sys_nice(int pid, int priority);

int sys_block(int pid);

void sys_mem();

int * sys_sem_open(char * name, int value);

void sys_sem_post(int * sema);

void sys_sem_wait(int * sema);

void sys_sem_close(int * sema);

int sys_get_curr_pid();

void sys_print_sem();

int sys_open_pipe(char * name);

void sys_print_pipes();

void sys_set_fd(int pid, int fdToModify, int newFd);

int sys_get_fd(int pid, int fdPos);

int sys_get_visibility(int pid);

#endif
