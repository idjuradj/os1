#ifndef _IVTENTRY_H_
#define _IVTENTRY_H_

#include "event.h"
#include "queue.h"

typedef void interrupt (*intRoutine)(...);

class IVTEntry{
	IVTNo entNmbr; // broj ulaza
	int callInd; // indikator poziva stare rutine

	intRoutine oldR; // stara prekidna rutina

	Queue* entryEvents;

	friend class Queue;
	friend class PCB;
	friend class KernelEv;
	friend class Event;

public:
	IVTEntry(intRoutine newR, IVTNo entry, int callold);

	~IVTEntry();

	static IVTEntry *ivTable[256];

	void signalize();

};

//PREPAREENTRY MAKRO
#define PREPAREENTRY(num, callOld)\
	void interrupt evInt##num(...);\
	IVTEntry ivtEntry##num(evInt##num,num,callOld);\
	void interrupt evInt##num(...){\
	IVTEntry::ivTable[##num]->signalize();\
 }

#endif