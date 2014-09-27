#include "system.h"
#include "thread.h"
#include "lockdef.h"
#include "schedule.h"
#include "pcb.h"
#include "ivtentry.h"
#include "kevent.h"
#include "event.h"
#include "ksem.h"
#include "semaphor.h"

#include "idle.h"
#include "mainth.h"

#include <stdlib.h>
#include <dos.h>
#include <stdio.h>
#include <iostream.h>

// ZA PROJEKAT

/* test1 */
#include "bounded.h"
#include "intLock.h"
#include "keyevent.h"
#include "user.h"

/* test2 */
//#include "AVG.H"
//#include "GEN.H"
//#include "intLock.h"
//#include "keyevent.h"
//#include "USER.h"

	volatile int dispInd = 0; // indikator poziva dispatch
	volatile int brojac = 5; // brojac za tajmer

	//pomocne promenljive za prekid tajmera 
	unsigned tsp, tss, tbp; 

	// stara/nova prekidna rutina
	//unsigned newSEG, newOFF, oldSEG, oldOFF; 

	volatile unsigned int lockFlag = 1; // zakljucavanje bez zabrane prekida
	intRoutine oldRoutine = getvect(0x08); //dohvatamo "staru" rutinu sa adrese 08h

void interrupt swc(...){
	//if (brojac == 0 || dispInd == 1){
		//if (lockFlag){
			dispInd = 0; //brise se flag za indikaciju zahteva za promenu konteksta
			asm{
				mov tsp, sp
				mov tss, ss
				mov tbp, bp
				
			}
			
			if(PCB::running != NULL){

				PCB::running->sp = tsp;
				PCB::running->ss = tss;
				PCB::running->bp = tbp;
			
				// nit koja se trenutno izvrsava stavljamo u red spremnih
				if (((PCB*)PCB::running)->threadState == RUNNING && ((PCB*)PCB::running)->threadId != 1) { 
					((PCB*)PCB::running)->threadState = READY;
					Scheduler::put((PCB*)PCB::running);
				}

			}
			
			// uzimamo jednu spremnu nit i menjamo joj status u running
			PCB::running = Scheduler::get();
			
			if(PCB::running!=NULL){
				((PCB*)PCB::running)->threadState = RUNNING;
			}
			else {
				if(PCB::allThreadsQ->isThereBlocked()){
					PCB::running = idleThread->thePCB;
				}
				
				else PCB::running = glavna->thePCB;
			}

			tsp = PCB::running->sp;
			tss = PCB::running->ss; 
			tbp = PCB::running->bp; 
			
			brojac = PCB::running->pcbTimeSlice;
			

			asm{
				mov sp, tsp
				mov ss, tss
				mov bp, tbp
			} 
		//} // kraj za if(lockFlag)
	
}

// dispatch() - eksplicitno/sinhrono preuzimanje
void dispatch(){
	lock
		dispInd = 1; 
		swc();
	unlock
}

void interrupt timer(...) {
	tick();
	(*oldRoutine)();

	PCB::sleepingQ->pqTimeTick();

	if(brojac!=0){
		if(--brojac == 0)swc();
	}

	
}

extern MainTh* glavna = NULL;
extern Idle* idleThread = NULL;

int main(int argc, char* argv[]){
	lock
		setvect(0x08,timer);
		idleThread = new Idle("idleThread");
		glavna = new MainTh("glavna");
		PCB::running = glavna->thePCB;
		PCB::running->threadState = RUNNING;
	unlock

	int rTm = 0;

		rTm = userMain(argc,argv);
	
		setvect(0x08,oldRoutine);

		glavna->thePCB = NULL;
		idleThread->thePCB = NULL;
		
	return rTm;
}