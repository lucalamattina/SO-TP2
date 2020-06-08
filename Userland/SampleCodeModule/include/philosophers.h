#include <shell.h>
#include "test_util.h"
#include <syscalls.h>

void initPhilos();
int addPhilo();
int removePhilo();
int center(int pos);
int left(int pos);
int right(int pos);
void check(int pos);
void takeForks(int pos);
void placeForks(int pos);
void philo();
int hungry();
void resetHungry();
void printBoard();
void philosophers();