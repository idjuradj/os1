#include "semaphor.h"
#include "ksem.h"
#include "lockdef.h"

#include <stdlib.h>
#include <dos.h>

//extern int semPreempt = 0;

Semaphore::Semaphore(int init){
	lock // zakljucavamo semafor
		myImpl = new KernelSem(init); // kreiramo novu instancu objekta tipa KernelSem
	unlock // otkljucavamo semafor
}

Semaphore::~Semaphore(){
	lock // zakljucavamo semafor
		delete myImpl;
	unlock // otkljucavamo semafor
}

//dovrsiti wait() metodu
int Semaphore::wait(){
	lock // zakljucavamo semafor
		int retVal = myImpl->wait(); // uzimamo vrednost koju wait() iz KernelSem vraca
	unlock // otkljucavamo semafor
	//if(semPreempt)dispatch();
	return retVal; // vracamo vrednost dobijenu iz KernelSem()
}

void Semaphore::signal(){
	lock // zakljucavamo semafor
		myImpl->signal();
	unlock // otkljucavamo semafor
	//if(semPreempt)dispatch();
}

int Semaphore::val() const { return myImpl->val(); }