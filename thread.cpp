#include "pcb.h"
#include "ksem.h"
#include "semaphor.h"
#include "thread.h"
#include "system.h"
#include "schedule.h"
#include "queue.h"
#include "pqueue.h"
#include "lockdef.h"
#include "iostream.h"

#include <dos.h>
#include <stdlib.h>


Thread::Thread(TName name, StackSize stackSize, Time timeSlice){
	lock
		myPCB = new PCB(name,stackSize,timeSlice, this);
	unlock
}

Thread::~Thread(){
	lock
		waitToComplete();
		delete myPCB;
	unlock
}

ID Thread::start(){ //returns: thread ID;
	lock
		myPCB->threadState = READY;
		myPCB->createPCBStack();
		Scheduler::put(myPCB);
	unlock
	return myPCB->threadId;
}

ID Thread::getId() {
	
		return myPCB->threadId;
	
}

TName Thread::getName() {
		return myPCB->threadName;
}

Thread* Thread::getThreadById(ID id){
	
		return PCB::allThreadsQ->threadWithGivenId(id);
	
}

ID Thread::getIdOf(TName name){
	
		return PCB::allThreadsQ->idOfGivenThread(name);

}

TName Thread::getName(ID id){

		return PCB::allThreadsQ->getTName(id);

}

int Thread::sleep(Time timeToSleep){
	//// returns: 1 - sleep completed, 0 - woken up
	//// wokenInd: 1 - nasilno probudjena, 0 - isteklo vreme za spavanje

	lock
		((PCB*)PCB::running)->threadState = SLEEP;
		PCB::sleepingQ->pqInsert(PCB::running, timeToSleep); // umetnemo u red uspavanih niti
	unlock
		
	dispatch();

	if(PCB::running->wokenInd) return 0; // ako je nit nasilno probudjena
	return 1; // ako je isteklo timeToSleep

	////dodato
	//return 0;
}

int Thread::wakeUp(ID id){
	int retVal;
	
	lock

		PCB* temp = PCB::allThreadsQ->pcbWithGivenId(id);

		if(temp == NULL)retVal = -1;

		else if(temp->threadState == SLEEP){
			temp->threadState = READY;
			temp->wokenInd = 1; // indikujemo da je nit nasilno probudjena.
			Scheduler::put(temp);
			PCB::sleepingQ->pqRemoveFromList(temp); // uklanjamo nit iz liste uspavanih
			retVal = 1;
		}

		else if(temp->threadState == BLOCKED){
			if(temp->blockingSem){ temp->unblockS();} // odblokiramo je na semaforu
			else if(temp->blockingEv) {temp->unblockEv();}// odblokiramo je na dogadjaju
			retVal = 1;
		}

		else retVal = 0;
	
	unlock

	return retVal;
}

int Thread::waitToComplete(ID id){
	//returns: 1 - run completed, 0 - woken up, -1 error
	int tempRET;

	lock
		Thread *tempThread = PCB::allThreadsQ->threadWithGivenId(id);
	unlock

	if(tempThread == NULL) return -1; //nema niti sa zadatim ID-em

	tempRET = tempThread->waitToComplete();
	return tempRET;

	////dodato
	//return 0;
}

int Thread::waitToComplete(){
	// returns: 1 - completed, 0 - woken up, -1 error
	int retVL;
	lock
		if(myPCB == NULL)retVL = -1;
		else if(myPCB->threadState == OVER)retVL=1;
		else if(myPCB->threadState == READY || myPCB->threadState == SLEEP || myPCB->threadState == BLOCKED){
			int sem = myPCB->blockedPCBs->wait();
			if(sem == 0)retVL=0; // ako je nit nasilno probudjena, vraca 0, pa waitToComplete vraca 0
			else retVL=1;
		}
		else retVL = 0;
	unlock

	return retVL;

	////dodato
	//return 0;
}

int Thread::waitForChildren(){
	lock
		while(PCB::running->childrenNum > 0){
			PCB::running->childrenSem->wait();
		}
	unlock
	return PCB::running->wokenInd;
}