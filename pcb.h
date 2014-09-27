// rad na pcb-u pocet 18.07.2013
#ifndef _PCB_H_
#define _PCB_H_

#include "thread.h"
#include "queue.h"
#include "pqueue.h"
#include "ksem.h"
#include "semaphor.h"
#include "kevent.h"

//proteklo vreme (za brojac)

// tip enum za odredjivanje stanja niti - READY (spremna), SLEEPY (uspavana), BLOCKED (blokirana)
typedef enum State {READY, BLOCKED, OVER, SLEEP, RUNNING, NEW};

class KernelEv;

class PCB {
private:
	static int threadIDCounter;
	static PCB* running; // PCB tekuce niti

	// elementi PCB-a
	ID threadId;
	TName threadName;
	State threadState;
	Thread* myThread;
	int wokenInd; // da li je nit probudjena pozivom WakeUp: 1 - jeste; 0 - nije;
	
	// vezano za promenu konteksta
	unsigned int ss, sp, bp; // ofset, segment, basepointer
	StackSize pcbStackSize; // velicina steka
	unsigned* stack; // pokazivac na vrh steka
	
	// vezano za timer
	Time pcbTimeSlice;

	// vezano za semafor(e)
	KernelSem* blockingSem; // pokazivac na kome se nit blokirala
	KernelSem* blockedPCBs; // niti blokirane pozivom waitToComplete
	KernelSem* childrenSem; // semafor za decu
	KernelEv* blockingEv; // dogadjaj na kome se nit blokira

	// vezano za waitForChildren - informacije o roditelju i deci
	PCB* parent;
	int childrenNum;
	
	// ulancane liste
	static PQueue* sleepingQ; // uspavane niti
	static Queue* allThreadsQ; // lista svih niti

	friend void interrupt timer(...);
	friend void interrupt swc(...);
	friend int main(int argc, char* argv[]); /////////////////////////////

public:
	// ostale funkcije
	void createPCBStack(); // kreiranje steka za PCB
	static void wrapper(); // funkcija omotac
	static void sleepingQTickTime(); // pristup tickTime za PCB - funkcija omotac

	//static void inic();

	void unblockS(); // rucno odblokira nit sa semafora
	void unblockEv(); // odblokira nit sa dogadjaja

	// konstruktor
	PCB(TName name, StackSize stackSize, Time timeSlice, Thread* theThrd);
	
	// destruktor
	~PCB();

	// prijateljska klasa, da bi mogla da pristupa running
	// i ostalim zasticenim poljima

	/* za testiranje */
	char* tName();
	ID tID();
	State tState();
	static void show();
	static void remove();
	
	friend class Thread;
	friend class Queue;
	friend class PQueue;
	friend class KernelSem;
	friend class KernelEv;
	friend class Semaphore;
	friend class IVTEntry;
	friend class Idle;
	friend class MainTh;

};

#endif