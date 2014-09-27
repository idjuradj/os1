#ifndef _MAINTH_H_
#define _MAINTH_H_

#include <iostream.h>
#include "thread.h"
#include "lockdef.h"
#include "semaphor.h"
#include "event.h"

class MainTh : public Thread {
private:
	PCB* thePCB;

	friend class PCB;
	friend class Thread;
	friend void interrupt timer(...);
	friend void interrupt swc(...);
	friend int main(int argc, char* argv[]);

public:
	MainTh(TName name, StackSize stackSize = defaultStackSize, Time timeSlice = defaultTimeSlice);
	
};

extern MainTh* glavna;

#endif