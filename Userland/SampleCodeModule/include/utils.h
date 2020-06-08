#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>
#include <stdarg.h>
#include <syscalls.h>
#include <stddef.h>
#include <naiveConsole.h>

#define SECONDS 0
#define MINUTES 1
#define HOURS 2
#define WEEKDAY 3
#define DAY_OF_MONTH 4
#define MONTH 5
#define YEAR 6
#define CENTURY 7

#define MAX_BUFFER 100

char getChar(void);
void putChar(char c);
void printf(char * str, ...);
int scanf(const char * fmt, ...);

char* itoa(int value, char* buffer, int base);

int atoi(const char* buffer, int len);

int isNumeric(char c);

char getKey(void);

void writeKey(char * key);

int strcmp(const char * stra, const char * strb);

int strncmp(const char * stra, const char * strb, size_t n);

int isAlpha(char c);

int strlen(const char * str);

void strncpy(char * dst, char * src, int n);

void strcpy(char * dst, char * src);

void makeSound(int freq, int time);

void goToSleep(int ticks);

void getTime(char * buff);

void getDate(char * buff);

void print(const char * str);

void printN(const char * str, int length);

void clearScreen();

void * malloc(size_t size);

void free(void * p);

int concat(char * to, const char * from);

#endif
