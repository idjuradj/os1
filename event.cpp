#include "event.h"
#include "semaphor.h"
#include "lockdef.h"
#include "kevent.h"

#include <stdlib.h>
#include <dos.h>

Event::Event(IVTNo ivtNo){
	lock
		myImpl = new KernelEv(ivtNo);
	unlock
}

Event::~Event(){
	lock
		delete myImpl;
	unlock
}

int Event::wait(){
	return myImpl->wait();
}

void Event::signal(){
	lock
		myImpl->signal();
	unlock
}