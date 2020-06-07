#ifndef SYSCALL_H
#define SYSCALL_H

#include <stdint.h>
#include <keyboard.h>
#include <time.h>
#include <io_read.h>
#include <sound.h>
#include <console.h>
#include <videoDriver.h>
#include <stddef.h>
#include <types.h>
#include <interrupts.h>
#include <MemoryManager.h>

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
#define OPENPIPE 25
#define PRINTPIPES 26
#define SETFD 27
#define GETFD 28
#define GETCURRPID 29

uint64_t handleSyscall(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9);

#endif
