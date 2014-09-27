#ifndef _KEVENT_H_
#define _KEVENT_H_

#include "event.h"
#include "ksem.h"

class Event;

class KernelEv{
	int intOcc;
	IVTNo ivtn;
	PCB* createdMe;


	
public:
	KernelEv(IVTNo ivtNo);
	~KernelEv();


	// returns: -1 pozvan wait na tudjem dogadjaju, 
	int wait();
	
	void signal();

//protected:
	friend class PCB;
	friend class IVTEntry;
};

#endif