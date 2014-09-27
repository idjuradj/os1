#include "ksem.h"
#include "lockdef.h"
#include "semaphor.h"
#include "system.h"

#include <stdlib.h>
#include <dos.h>

KernelSem::KernelSem(int value = 1){ 
	lock
		semval = value;
		waitingQ = new Queue();
	unlock
}
KernelSem::~KernelSem(){ 
	lock
		delete waitingQ;
	unlock
}

void KernelSem::incVal(){ semval++; }

void KernelSem::signal() {
	lock
		// "signaliziramo" semafor, stoga uvecavamo vrednost brojaca semval;
		semval++;
	
		// ako je semval <0, postoji bar jedna nit blokirana na semaforu, 
		// pa je iz reda blokiranih prebacujemo u red spremnih
		if(semval <= 0){
			PCB* temp = waitingQ->get(); // da li ce red blokiranih niti biti tako nazvan, uzimamo nit iz reda blokirajucih niti.
			((PCB*)temp)->threadState = READY; // menjamo joj status u READY, posto je postala "spremna"
			((PCB*)PCB::running)->blockingSem = NULL; // nit se odblokirala, "ponistavamo" semafor.
			((PCB*)PCB::running)->wokenInd = 0;
			Scheduler::put(temp); // "spremnu" nit stavljamo u red niti spremnih za izvrsavanje.
		}
	
		// posto semPreempt ukazuje na to da li prilikom svakog poziva neke operacije semafora
		// treba ili ne treba vrsiti preuzimanje, ovde to proveravamo. Ukoliko je data vrednost
		// razlicita od 0, vrsi se preuzimanje, u suprotnom ne.
		if(semPreempt)dispatch();
	unlock
}

int KernelSem::wait(){
	// ukoliko je nit deblokirana pozivom operacije signal() - vraca se 1 (vrednost != 0)
	// u suprotnom rezultat ima vrednost 0.

	lock
		// "signaliziramo" da nam treba semafor, pa umanjujemo
		//	vrednost semaforskog brojaca semval
		semval--;
	
		int rV = 1; //rV ostaje nula ukoliko wokenInd tekuce niti !=1;

		// ako se nit blokirala na semaforu, stavljamo je u red blokiranih
		if(semval < 0){
			((PCB*)PCB::running)->threadState = BLOCKED;
			((PCB*)PCB::running)->blockingSem = this; // pamtimo semafor na kome se nit blokirala
			waitingQ->put(PCB::running);
			dispatch();

			if(PCB::running->wokenInd)rV=0; // ako je nit nasilno probudjena vracamo vrednost 0.
		}
	
		// posto semPreempt ukazuje na to da li prilikom svakog poziva neke operacije semafora
		// treba ili ne treba vrsiti preuzimanje, ovde to proveravamo. Ukoliko je data vrednost
		// razlicita od 0, vrsi se preuzimanje, u suprotnom ne.
		else if(semPreempt)dispatch();
	unlock
	
	return rV;
}

int KernelSem::val() const { return semval; }