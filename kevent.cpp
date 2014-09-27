#include "kevent.h"
#include "event.h"
#include "ivtentry.h"
#include "lockdef.h"
#include "iostream.h"
#include "pcb.h"

KernelEv::KernelEv(IVTNo ivtNo){
	lock
		ivtn = ivtNo;
		intOcc = 0;
		createdMe = PCB::running;
		IVTEntry::ivTable[ivtn]->entryEvents->put(PCB::running);
	unlock
}

KernelEv::~KernelEv(){
	lock
		IVTEntry::ivTable[ivtn]->entryEvents->removeFromList(PCB::running);
		createdMe = NULL;
	unlock
}

int KernelEv::wait() {
	if(PCB::running != createdMe ){	return -1;}
	
	lock
	if(!intOcc){
		PCB::running->threadState = BLOCKED;
		PCB::running->blockingEv = this;
		dispatch();	
	}

	else {
		intOcc = 0; // resetuje se fleg da se desio prekid 
	}

	unlock
	// ako je nit deblokirana pozivom signal, wokenInd je 0, i vraca se vrednost !=0;
	if(PCB::running->wokenInd == 0) return 1; 
	else return 0; 
}

void KernelEv::signal(){
	
	if(createdMe->threadState == BLOCKED){
			((PCB*)createdMe)->threadState = READY;
			((PCB*)createdMe)->blockingEv = NULL;
			((PCB*)createdMe)->wokenInd = 0;
			Scheduler::put(createdMe);
	}
	else { intOcc = 1; }
}