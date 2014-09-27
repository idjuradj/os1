#include "ivtentry.h"
#include "event.h"
#include "kevent.h"
#include "pcb.h"
#include <iostream.h>

IVTEntry* IVTEntry::ivTable[256];

IVTEntry::IVTEntry(intRoutine newR, IVTNo entry, int callold){
	lock
		oldR = getvect(entry);
		entNmbr = entry;
		setvect(entry,newR);
		IVTEntry::ivTable[entry] = this;
		callInd = callold;
		entryEvents = new Queue();
	unlock

}

IVTEntry::~IVTEntry(){
	lock
		setvect(entNmbr,oldR);
		delete entryEvents;
		IVTEntry::ivTable[entNmbr]=0;
		
		
		asm{
			mov al, 20h
			out 20h, al
		}

	unlock
}

void IVTEntry::signalize(){
	// ako treba, pozvati staru rutinu
	if(callInd)(oldR)();

	// signalizirati svim dogadjajima na prekidu
	for(Node* temp = entryEvents->head; temp != NULL; temp = temp->next){

		// provera za slucaj da se u nasilnom budjnu setovao na nulu. Just in case.
		if(temp->pcb->blockingEv){ 
			temp->pcb->blockingEv->signal();
		}
	}

	dispatch();
}