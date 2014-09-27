#include "idle.h"
#include "thread.h"
#include "pcb.h"

Idle::Idle(TName name, StackSize stackSize, Time timeSlice) : Thread(name, stackSize, timeSlice){
	lock
		thePCB = PCB::allThreadsQ->pcbWithGivenId(this->getId());

		thePCB->createPCBStack();
		thePCB->threadState = READY;
		thePCB->parent = NULL;
	unlock
};

void Idle::run() { while(1); }