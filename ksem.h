#ifndef _KSEM_H_
#define _KSEM_H_

#include "queue.h"
#include "pcb.h"

class KernelSem {
private:
	int semval;
	Queue* waitingQ;

	friend class PCB;
	
public:
	//konstruktor,destruktor
	KernelSem(int value);
	~KernelSem();
	
	//wait(),signal()
	void signal();
	int wait();
	
	//vrednost semafora
	int val() const;
	void incVal();
	
	friend class Semaphore;
	
};

#endif