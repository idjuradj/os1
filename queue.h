// klasa Queue, kojom se predstavlja red pomocu ulancanih lista

#ifndef _QUEUE_H_
#define _QUEUE_H_

#include "lockdef.h"
#include "thread.h"
#include "pcb.h"
#include "queue.h"
#include "system.h"
#include "schedule.h"

#include <dos.h>
#include <stdlib.h>

//   Struktura cvora
struct node
{
   PCB* pcb;
   struct node *next;

   node(){}
   ~node(){pcb = NULL; next = NULL;}
};

typedef struct node Node;
 
// Klasa cvora
class Queue{
private:
   Node *head,*tail;
   int numElem; // duzina liste
   
   friend class PQueue;
   friend class PCB;
   friend class IVTEntry;

public:
	// konstruktor, destruktor
    Queue();
	~Queue();
	
	// metode za rad sa redom
    void put(PCB* p); // umetanje elementa
	PCB* get(); // // "vadjenje" prvog elementa liste, zasad nepotrebno?
	PCB* pcbWithGivenId(ID i); // vratiti pcb datog id-a
    void del();  // brisanje elementa
    void removeFromList(PCB* pcb); // brisanje odredjenog elementa iz liste
	
	// trazenja odgovarajuce niti u redu
	Thread* threadWithGivenId(ID theId);
	ID idOfGivenThread(TName theName);
	TName getTName(ID theId);
	
	int numEl() const; // ispisivanje duzine reda
	int isThereBlocked();

	/* ZA TESTIRANJE */
	void lshow();
	void lremove();
};

#endif