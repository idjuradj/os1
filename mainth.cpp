#include "mainth.h"
#include "thread.h"
#include "pcb.h"

MainTh::MainTh(TName name, StackSize stackSize, Time timeSlice)
	:Thread(name, stackSize, timeSlice) {
	lock
		thePCB = PCB::allThreadsQ->pcbWithGivenId(this->getId());
	unlock
}