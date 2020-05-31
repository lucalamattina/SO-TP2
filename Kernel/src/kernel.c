#include <stdint.h>
//#include <string.h>
#include <strings.h>
#include <lib.h>
#include <moduleLoader.h>
#include <naiveConsole.h>
#include <irqDispatcher.h>
#include <time.h>
#include <keyboard.h>
#include <idtLoader.h>
#include <interrupts.h>
#include <syscall.h>
#include <videoDriver.h>
#include <console.h>
#include <pixelMap.h>
#include <exceptions.h>
#include <MemoryManager.h>
#include <process.h>
#include <scheduler.h>


extern uint8_t text;
extern uint8_t rodata;
extern uint8_t data;
extern uint8_t bss;
extern uint8_t endOfKernelBinary;
extern uint8_t endOfKernel;

static const uint64_t PageSize = 0x1000;

//Addresses a donde copia los modulos
static void * const sampleCodeModuleAddress = (void*)0x400000;
static void * const sampleDataModuleAddress = (void*)0x500000;
static uint64_t * const free_list_address = (uint64_t *) 0x1000000;

typedef int (*EntryPoint)();


void clearBSS(void * bssAddress, uint64_t bssSize)
{
	memset(bssAddress, 0, bssSize);
}

void * getStackBase()
{
	return (void*)(
		(uint64_t)&endOfKernel
		+ PageSize * 8				//The size of the stack itself, 32KiB
		- sizeof(uint64_t)			//Begin at the top of the stack
	);
}

void goToUserland(){
	((EntryPoint)sampleCodeModuleAddress)();
}

void * initializeKernelBinary()
{
	char buffer[10];

	cpuVendor(buffer);

	//Llena con los addresses a donde copia los modulos
	void * moduleAddresses[] = {
		sampleCodeModuleAddress,
		sampleDataModuleAddress
	};

	loadModules(&endOfKernelBinary, moduleAddresses);

	clearBSS(&bss, &endOfKernel - &bss);

	print("Starting video driver\n");
  initVideoDriver();
	print("Starting console\n");
  init_console();
	print("Starting memory manager\n");
	initializeFreeList(free_list_address); //Initialize Memory Manager with Free list
	print("Starting processes\n");
	initList(); //Initialize Process List
	print("Starting scheduler\n");
	initScheduler();
	print("Loading IDT\n");
 	load_idt();
	print("Loading exceptions\n");
	loadExceptions();

	return getStackBase();
}

//void startShell(){
//	process* shell= newProcess("shell",0,NULL, 10, FOREGROUND, (uint64_t) sampleCodeModuleAddress);
//	setState(shell->pid, RUNNING);
//	_runProcess(shell->stackPointer);
//}

int main(){
	// goToUserland();

	//process * init = newProcess("init",0,NULL, 10, BACKGROUND, (uint64_t) startShell);
	//setState(init->pid, RUNNING);
	//fakeStack(init);
	//_runProcess(init->stackPointer);
	return 1;

}
