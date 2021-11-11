#include "scheduler.h"

extern THANDLER threads[MAXTHREAD];
extern int currthread;
extern int blockevent;
extern int unblockevent;

QUEUE ready;
QUEUE waitinginevent[MAXTHREAD];

int q = 0; //quantum

void scheduler(int arguments) {

	int old,next;
	int changethread=0;
	int waitingthread=0;
	int event=arguments & 0xFF00;
	int callingthread=arguments & 0xFF;

	if(event==NEWTHREAD) {

		// Un nuevo hilo va a la cola de listos
		threads[callingthread].status=READY;
		_enqueue(&ready,callingthread);
	}

	if(event==BLOCKTHREAD) {

		threads[callingthread].status=BLOCKED;
		_enqueue(&waitinginevent[blockevent],callingthread);

		changethread=1;
	}

	if(event==ENDTHREAD) {

		threads[callingthread].status=END;
		changethread=1;
	}

	if(event==UNBLOCKTHREAD) {

		threads[callingthread].status=READY;
		_enqueue(&ready,callingthread);
	}
//////////////////////// ROUND ROBIN QUANTUM == 2 ///////////////////////////
	if(event==TIMER) {//comparacion del evento al tiempo 
		q++;  //incrementar el quantum
		if(q == 2) {// si quantum == 2

		_enqueue(&ready,callingthread); //Se pondra listo hasta que el quantum permita
		next = _dequeue(&ready);  //seleccionamos al siguiente y lo ponemos en listo
		swapthreads(current, next); //cambia el hilo actual por el siguiente 
		}
	}
/////////////////////////////////////////////////////////////////////////////
	if(changethread) {

		old=currthread;
		next=_dequeue(&ready);

		threads[next].status=RUNNING;
		_swapthreads(old,next);
	}

}
