#ifndef SHELL_H
#define SHELL_H

#include <stdint.h>
#include <naiveConsole.h>
#include <utils.h>
#include <types.h>
#include <stddef.h>
#include <philosophers.h>

#define MAX_BUFF_SIZE 750

//Address original que devolvia el samplecoremodule
#define RETURN_ADRESS 0xDEADC0DE

uint64_t * init_shell(void);

void handle_command(int cmd);

void display_welcome_message(void);

void display_help(void);

void display_goodbye_message(void);

void clear_buffer(char * buff);

void display_time(void);

void display_date(void);

void make_sound(void);

void sleep(void);

void display_invalid_command(void);




void ps();

int nice(int nicepid, int nicepriority);

int kill(int pid);

int block(int blockpid);

#endif
