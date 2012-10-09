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

int user_main(int argc, char **argv)
{
	thrd_t t2, t3;

	barr = new spinning_barrier(2);

	thrd_create(&t2, &threadA, NULL);
	thrd_create(&t3, &threadB, NULL);
	thrd_join(t2);
	thrd_join(t3);

	return 0;
}
