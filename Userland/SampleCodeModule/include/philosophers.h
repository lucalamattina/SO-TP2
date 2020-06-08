<<<<<<< HEAD
=======
#include <shell.h>
>>>>>>> 6f9112db3554f64ea7fdaefe96095d2b6a7eab8b
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
