#ifndef _SYSTEM_H_
#define _SYSTEM_H_

#include "lockdef.h"
#include "thread.h"
#include "pcb.h"
#include "queue.h"
#include "pqueue.h"
#include "schedule.h"

#include <dos.h>
#include <stdlib.h>

	int userMain(int argc, char* argv[]);
	int main(int argc, char* argv[]);
	
	extern void tick();

	extern volatile int dispInd; // indikator poziva dispatch
	extern volatile int brojac; // brojac za tajmer
	extern volatile unsigned int lockFlag;
	
	void inic();
	void restore();
	
	void dispatch(); // "sinhrono" preuzimanje
	void interrupt timer(...); // timer prekidna rutina 
	void interrupt swc(...);


#endif 