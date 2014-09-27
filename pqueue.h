// klasa PQueue - prioritetni red uspavanih lista
// sa cvorom koji sadrzi pcb, timetosleep i pokazivac next 

#ifndef _PQUEUE_H_
#define _PQUEUE_H_

#include "lockdef.h"
#include "thread.h"
#include "pcb.h"
#include "queue.h"
#include "system.h"
#include "schedule.h"

#include <dos.h>
#include <stdlib.h>

//   Struktura cvora
struct pnode
{
   PCB* pcb;
   //Time sleepTime;
   Time timeDiff;
  // Time spentSleeping;
   struct pnode *next;
   
   pnode(PCB* p, pnode* nnext=NULL): pcb(p), timeDiff(0), next(nnext) {}
   ~pnode(){ pcb = NULL; next = NULL; }
};

typedef struct pnode PNode;

class PQueue {
private:
	PNode *pqHead, *pqTail;

	friend class Queue;
	friend class PCB;
	
public:
	PQueue(); // konstruktor

	void pqInsert(PCB* p, Time timeToSleep); // umetanje
	void pqDel(); // brisanje liste
	void pqRemoveFromList(PCB* p); // brisanje odredjenog elementa 
	void pqTimeTick(); // potok elementa
	
	int wakeTUp(PCB* pcb);	
	PCB* pcbWithGivenId(ID i);
	
	~PQueue(); // destruktor

	/* ZA TESTIRANJE */
	void plshow();

}; // end of class PNode

#endif