#include <scheduler.h>

extern THANDLER threads[MAXTHREAD];
extern int currthread;
extern int blockevent;
extern int unblockevent;
#define PRIORITYNUMBER 20 //El número de prioridades que tendremos
QUEUE prior[PRIORITYNUMBER];
QUEUE ready;
QUEUE waitinginevent[MAXTHREAD];
int currentPriority = 0;
int nthreads = 0;
int init = 0;
void scheduler(int arguments)
{
	int old, next;
	int changethread = 0;
	int waitingthread = 0;
	int event = arguments & 0xFF00;
	int callingthread = arguments & 0xFF;

	if (event == NEWTHREAD)
	{
		// Un nuevo hilo va a la cola de listos
		threads[callingthread].status = READY;
		_enqueue(&prior[0], callingthread); //Aqui hacemos que el thread que llama sea puesto en máxima prioridad en la cola
		//_enqueue(&ready,callingthread); No se pondra listo aun, hasta que se permita en tiempo
		if (init == 0)
		{
			init = 1;
		}
		else
		{
			nthreads++; //Indica que tendremos un nuevo hilo
		}
	}

	if (event == BLOCKTHREAD)
	{

		threads[callingthread].status = BLOCKED;
		_enqueue(&waitinginevent[blockevent], callingthread);
		//Procedemos a revisar si hay elementos en las prioridades
		for (int i = 0; i < PRIORITYNUMBER; i++)
		{
			if (_emptyq(&prior[i]) == 0)
			{
				currentPriority = i;				   //Esto nos indica la prioridad con la que trabajaremos
				_enqueue(&ready, _dequeue(&prior[i])); //Encolamos el siguiente elemento
				changethread = 1;
				break;
			}
		}
	}
	if (event == ENDTHREAD)
	{
		nthreads--;
		threads[callingthread].status = END;
		for (int i = 0; i < PRIORITYNUMBER; i++)
		{
			if (_emptyq(&prior[i]) == 0)
			{
				//currentPriority = i; //Esto nos indica la prioridad con la que trabajaremos
				_enqueue(&ready, _dequeue(&prior[i])); //Encolamos el siguiente elemento
				changethread = 1;
				break;
			}
		}
	}

	if (event == UNBLOCKTHREAD)
	{
		threads[callingthread].status = READY;
		_enqueue(&ready, callingthread);
		//Determinamos la prioridad minima
		for (int i = 0; i < PRIORITYNUMBER; i++)
		{
			if (_emptyq(&prior[i]) == 0)
			{
				currentPriority = i;
				break;
			}
		}

		if (currentPriority == PRIORITYNUMBER - 1)
		{
			_enqueue(&prior[PRIORITYNUMBER - 1], callingthread);
		}
		else
		{
			_enqueue(&prior[currentPriority + 1], callingthread);
		}
	}
	//Si es timer tenemos que cambiar el thread, además de mandarlo a la siguiente cola
	if (event == TIMER)
	{
		if (nthreads > 0) //Si el thread es 0, no generamos cambio
		{
			//Determinamos la prioridad minima
			for (int i = 0; i < PRIORITYNUMBER; i++)
			{
				if (_emptyq(&prior[i]) == 0)
				{
					currentPriority = i;
					break;
				}
			}
			if (currentPriority == PRIORITYNUMBER - 1) //Comprobamos si ya alcanzamos el ultimo nivel de prioridad
			{
				_enqueue(&prior[PRIORITYNUMBER - 1], callingthread); //Volvemos a encolar al thread que está en la maxima prioridad
			}
			else //Esto significa que no estamos en la ultima prioridad
			{
				_enqueue(&prior[currentPriority + 1], callingthread);
			}
			_enqueue(&ready, _dequeue(&prior[currentPriority])); //Encolamos el item de la cola
			threads[callingthread].status = READY;
			changethread = 1;
		}
	}

	if (changethread)
	{
		old = currthread;
		next = _dequeue(&ready);

		threads[next].status = RUNNING;
		_swapthreads(old, next);
	}
}