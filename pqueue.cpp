#include "lockdef.h"
#include "thread.h"
#include "pcb.h"
#include "queue.h"
#include "system.h"
#include "schedule.h"
#include "iostream.h"

PQueue::PQueue(){
	lock
		pqHead = NULL;
	unlock
}

PQueue::~PQueue(){
	lock
		pqDel();
	unlock
}

void PQueue::pqInsert(PCB* pcb, Time timeToSleep){
lock
		if(pcb != NULL && timeToSleep > 0){
		
                PNode* temp0 = NULL;
                PNode* temp1 = pqHead;
                PNode* temp = new PNode(pcb);
                Time sum = 0;
               
                // dok je suma manja i dok nismo dosli do kraja liste
                while(temp1!= NULL && sum + temp1->timeDiff < timeToSleep){
                        sum+=temp1->timeDiff;
 
                        temp0=temp1;
                        temp1=temp1->next;
                }
 
       
                temp->timeDiff = timeToSleep - sum;
                temp->next = temp1;
                if (temp0 != 0)                                                                                          
                        temp0->next = temp;                                                                  
                else
                        pqHead = temp;
       
                if (temp1 != 0)
                        temp1->timeDiff -= temp->timeDiff;
		
		}
unlock
}

void PQueue::pqDel(){
		PNode *temp;
		while(pqHead){
			temp = pqHead;
			pqHead = pqHead->next;
			delete temp;
		}
}

void PQueue::pqTimeTick(){
	//lock

		//PNode* temp = pqHead;
		//while(temp){
		//	temp->spentSleeping++;
		//	if(temp->spentSleeping >= temp->sleepTime){
		//		temp->pcb->threadState = READY;
		//		temp->pcb->wokenInd = 0; // nit se probudila jer joj je isteklo vreme za spavanje
		//		Scheduler::put(temp->pcb);
		//		PNode *forRemoval = temp;
		//		pqRemoveFromList(forRemoval->pcb);
		//	}
		//	temp = temp->next;
		//}

		PNode *temp = pqHead;
		
		temp->timeDiff--; // umanjimo vreme head-a


		while(temp!=NULL && temp->timeDiff==0){
			temp->pcb->threadState = READY;
			temp->pcb->wokenInd = 0; // nit se probudila jer joj je isteklo vreme za spavanje
			Scheduler::put(temp->pcb);
			PNode *forRemoval = temp;
			temp = temp->next;

			pqRemoveFromList(forRemoval->pcb);
		}
	//unlock
}

void PQueue::pqRemoveFromList(PCB* p){
	lock
		PNode* n1 = pqHead;
		PNode* n0 = NULL;

		while(n1!=NULL && n1->pcb != p){
			n0 = n1;
			n1 = n1->next;
		}

		// ako je n1==NULL, doslo se do kraja liste, u kojoj nema trazenog elementa
		if(n1==NULL)return; 

		// ako je n0==NULL, brise se prvi element
		if(n0 == NULL){
			PNode* old = n1;
			pqHead = pqHead->next;
			n1 = n1->next;
			if(pqHead!=NULL && p->wokenInd!=0)pqHead->timeDiff += old->timeDiff;
			delete old;
		}

		// u suprotnom, brise se neki element iz sredine/sa kraja liste
		else {
			PNode* old = n1;
			n1 = n1->next;
			n0->next = n1;
			if(n1!=0 && p->wokenInd!=0)n1->timeDiff += n0->timeDiff;
			delete old;
		}
	unlock

}

int PQueue::wakeTUp(PCB* ppcb){

	// ako je nit uspavana, budimo je tako sto menjamo status u ready i
	// idnikujemo da je "nasilno" probudjena i stavljamo u Scheduler
	// vracamo 1, jer je nit probudjena
	//lock
		ppcb->threadState = READY;
		ppcb->wokenInd = 1; // indikujemo da je nit nasilno probudjena.
		Scheduler::put(ppcb);
		PCB::sleepingQ->pqRemoveFromList(ppcb); // uklanjamo nit iz liste uspavanih
	//unlock
	return 1;
}

PCB* PQueue::pcbWithGivenId(ID theId){
	PNode* temp = pqHead;
	while(temp){
		if(temp->pcb->threadId == theId) return temp->pcb;
		else temp = temp->next;
	}
	return NULL;
}

void PQueue::plshow(){
	PNode* temp = pqHead;
	while(temp){
		cout << temp->pcb->tName() << " " << temp->pcb->tID() << endl;
		temp = temp->next;
	}
}