#include <shell.h>
#include "test_util.h"
#include <syscalls.h>

void initPhilos();
int addPhilo();
int removePhilo();
int left(int i, int mod);
int right(int i, int mod);
void check(int pos);
void takeForks(int pos);
void placeForks(int pos);
void philo();
int hungry();
void resetHungry();
void printTable();
void philosophers();