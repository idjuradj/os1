#ifndef _idle_h_
#define _idle_h_

#include "thread.h"
#include "pcb.h"

class Idle : public Thread {

public: 
	Idle(TName name, StackSize stackSize = defaultStackSize, Time timeSlice = defaultTimeSlice);

	void start();

protected:
	void run();

private:
	PCB* thePCB;

	friend class PCB;
	friend class Thread;
	friend void interrupt timer(...);
	friend void interrupt swc(...);
	friend int main(int argc, char* argv[]);

};

extern Idle* idleThread;

#endif