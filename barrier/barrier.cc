#include <stdio.h>
#include <threads.h>

#include "barrier.h"

#include "librace.h"

spinning_barrier *barr;
int var = 0;

void threadA(void *arg)
{
	store_32(&var, 1);
	barr->wait();
}

void threadB(void *arg)
{
	barr->wait();
	printf("var = %d\n", load_32(&var));
}

#define NUMREADERS 1
int user_main(int argc, char **argv)
{
	thrd_t A, B[NUMREADERS];
	int i;

	barr = new spinning_barrier(NUMREADERS + 1);

	thrd_create(&A, &threadA, NULL);
	for (i = 0; i < NUMREADERS; i++)
		thrd_create(&B[i], &threadB, NULL);

	for (i = 0; i < NUMREADERS; i++)
		thrd_join(B[i]);
	thrd_join(A);

	return 0;
}
