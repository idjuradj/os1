// cpp fajl za Queue

#include "queue.h"
#include "lockdef.h"
#include "thread.h"
#include "pcb.h"
#include "system.h"
#include "schedule.h"
#include "string.h"

#include <stdlib.h>
#include <dos.h>
#include <stdio.h>
#include <iostream.h>


Queue::Queue(){
	lock
		head = tail = NULL;
		numElem = 0;
	unlock
}
 
Queue::~Queue(){
	lock
		Queue::del();
	unlock
}

// stavlja element u red (na kraj)
void Queue::put(PCB* p){
	//lock
		Node* temp = new Node();
		temp->pcb = p;
		temp->next = NULL;
		
		if(!head)head = temp;
		else tail->next = temp;
		
		tail = temp;
	//unlock
}

// dohvata prvi element reda
PCB* Queue::get(){
	if(!head){ return NULL; }
	lock
		PCB* temp = head->pcb;
		Node* old = head;
		head = head->next;
		delete old;
		numElem--;
	unlock
	
	return temp;
}

// brise elemente reda
void Queue::del(){
	//lock
		Node *temp;
			while(head){
				temp = head;
				head = head->next;
				delete temp;
			}
		numElem = 0;
	//unlock
}  // brisanje elementa

// vraca informaciju o broju elemenata u redu, nepotrebno?
int Queue::numEl() const {return numElem;}

// vraca nit na osnovu zadatog ID-a
Thread* Queue::threadWithGivenId(ID theId){
	Node* temp = head;
	while(temp){
		if(temp->pcb->threadId == theId) return temp->pcb->myThread;
		else temp = temp->next;
	}
	return NULL;
}

// vraca ID niti na osnovu imena -- odraditi pomocu strcmp
ID Queue::idOfGivenThread(TName theName){
	Node* temp = head;
	while(temp){
		if(!strcmp(temp->pcb->threadName,theName)) return temp->pcb->threadId;
		else temp = temp->next;
	}
	return NULL;
}

TName Queue::getTName(ID theId){
	Node* temp = head;
	while(temp){
		if(temp->pcb->threadId == theId) return temp->pcb->threadName;
		else temp = temp->next;
	}
	return NULL;
}

void Queue::removeFromList(PCB* p){
	Node* n0 = NULL;
	Node* n1 = head;
	
	while(n1 != NULL){ // krecemo se kroz listu
		if(p == n1->pcb){
			if(n0 == NULL){ // znaci da je p = head
				if (n1 == tail) tail = NULL; 	
				head = head->next;					
			}
		
		
			else {
				if (n1 == tail) tail = n0;		
				n0->next = n1->next;					
			}
			
			delete n1; break;
		}
		else{
			n0 = n1;
			n1 = n1->next;
		}
	
	} // kraj while
}

PCB* Queue::pcbWithGivenId(ID theId){
	Node* temp = head;
	while(temp){
		if(temp->pcb->threadId == theId) return temp->pcb;
		else temp = temp->next;
	}
	return NULL;
}

int Queue::isThereBlocked(){
	Node* temp = head;
	while(temp){
		if(temp->pcb->threadState == BLOCKED || temp->pcb->threadState == SLEEP) return 1;
		temp = temp->next;
	}

	return 0;
}


/* TESTIRANJE */

void Queue::lshow(){
	Node* temp = head;
	while(temp){
		cout << temp->pcb->tName() << " " << temp->pcb->tID() << endl;
		temp = temp->next;
	}
}

void Queue::lremove(){
	PCB* forRemoval = pcbWithGivenId(3);
	removeFromList(forRemoval);
}