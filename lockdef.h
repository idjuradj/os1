#ifndef _LOCKDEF_H_
#define _LOCKDEF_H_

// zakljucavanje:
#define lock asm{\
	pushf;\
	cli;\
}

// otkljucavanje:
#define unlock asm popf

// Zabrana ispisa
#define lockCout lockFlag=0;

// Dozvola ispisa
#define unlockCout lockFlag=1;\
	if(dispInd){\
	dispatch();\
}

#endif