#include <irqDispatcher.h>
#include <stdint.h>
#include <time.h>
#include <naiveConsole.h>
#include <keyboard.h>
#include <scheduler.h>

void irqDispatcher(uint64_t irq,  uint64_t stackPointer);
void int_20(uint64_t stackPointer);
void int_21();
extern int read_key();

void irqDispatcher(uint64_t irq, uint64_t stackPointer) {
    switch (irq) {
        case 0:
          int_20(stackPointer);
		         break;
        case 1:
          int_21();
          break;
	}
	return;
}

//Interrupcion para el timer tick
void int_20(uint64_t stackPointer) {
    timer_handler();
    //scheduler(stackPointer);
}

//Interrupcion para el teclado
void int_21() {
    keyboard_handler();
}
