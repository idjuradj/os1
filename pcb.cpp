// CPP za PCB
#include <stdlib.h>
#include <dos.h>
#include <string.h>
#include <iostream.h> // za sprintff
#include <stdio.h>

#include "lockdef.h"
#include "queue.h"
#include "pcb.h"
#include "thread.h"
#include "system.h"
#include "schedule.h"
#include "mainth.h"
#include "idle.h"


int PCB::threadIDCounter = 0;
PQueue* PCB::sleepingQ = new PQueue();
Queue* PCB::allThreadsQ = new Queue();
PCB* PCB::running = NULL;

PCB::PCB(TName name, StackSize stackSize, Time timeSlice, Thread* theThrd){
	lock
		threadId = ++threadIDCounter;
		wokenInd = 0;

		if(stackSize > 32768)stackSize = 32768;
		else pcbStackSize = stackSize;
		
		stack = NULL;
		threadState = NEW;
		
		pcbTimeSlice = timeSlice;
		
		if(name == 0){
			threadName = new char[20];
			sprintf (threadName, "thread%d", threadId);
		}
		else{
			threadName = new char[strlen(name)+1];	
			strcpy (threadName, name);
		}
		
		myThread = theThrd;
		
		//ako je threadId > 0, kreirana je nova nit, stavlja se u red svih niti
		if(threadId) allThreadsQ->put(this);

		childrenSem = new KernelSem(0);

		parent = PCB::running;
		if(parent != NULL) parent->childrenNum++;

		blockedPCBs = new KernelSem(0);
		blockingSem = NULL;
		blockingEv = NULL;



	unlock
}

PCB::~PCB(){
	lock
		// izbaciti iz liste svih PCB-ova
		// obrisati stack, listu dece, semafore, niz
		// i to je to

		PCB::allThreadsQ->removeFromList(this);
		if(stack)delete stack;
		if(threadName)delete threadName;
		if(blockedPCBs) delete blockedPCBs;
		if(blockingSem) delete blockingSem;
		if(blockingEv) delete blockingEv;

	unlock
}

void PCB::createPCBStack(){
	/* kod dat na lab-u */

	lock //zakljucavamo
		stack = new unsigned[pcbStackSize]; // stek
				
		// stavljamo na stek: PSW, pa segment i offset wrapper funkcije
		// sp,bp,ss postavljamo 12 mesta od pocetka steka
		stack[pcbStackSize - 1] = 0x200;
		stack[pcbStackSize - 2] = FP_SEG(PCB::wrapper);
		stack[pcbStackSize - 3] = FP_OFF(PCB::wrapper); 

		sp = FP_OFF(stack + pcbStackSize - 12);
		bp = FP_OFF(stack + pcbStackSize - 12);
		ss = FP_SEG(stack + pcbStackSize - 12);

	unlock // otkljucavamo
}

void PCB::wrapper(){
	PCB::running->myThread->run();
	lock
		// wfc stanje wtc
		
		
		if(PCB::running->parent){
			PCB::running->parent->childrenNum--;
			PCB::running->parent->childrenSem->signal();
		}

		((PCB*)PCB::running)->threadState = OVER;

		//signaliziramo nitima blokiranih na ovoj niti da je gotova
		for(int i = PCB::running->blockedPCBs->val(); i<0; i++)PCB::running->blockedPCBs->signal();

	unlock
	
	dispatch();
}

void PCB::sleepingQTickTime(){
	sleepingQ->pqTimeTick();
}

void PCB::unblockS(){
	wokenInd = 1;
	blockingSem->incVal();
	blockingSem->waitingQ->removeFromList(this);
	threadState = READY;
	Scheduler::put(this);
}

void PCB::unblockEv(){
	threadState = READY;
	blockingEv = NULL;
	wokenInd = 1;
	Scheduler::put(this);
}

//void PCB::inic(){
//
//}

/* za testiranje */
void PCB::show(){
	allThreadsQ->lshow();
}

void PCB::remove(){
	allThreadsQ->get();
}

char* PCB::tName(){
	return threadName;
}

ID PCB::tID(){
	return threadId;
}

State PCB::tState(){
	return threadState;
}
