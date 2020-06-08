#include <shell.h>
#include "test_util.h"
#include <syscalls.h>

void initPhilos();
int addPhilo();
int removePhilo();
int center(int pos);
int left(int pos);
int right(int pos);
void getForks(int pos);
void eat(int pos);
void finishEating(int pos);
void philo();
int hungry();
void resetHungry();
void printBoard();
void philosophers();
