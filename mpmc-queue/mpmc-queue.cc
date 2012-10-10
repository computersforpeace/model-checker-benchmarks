#include <threads.h>
#include <stdio.h>

#include <librace.h>

#include "mpmc-queue.h"

void threadA(struct mpmc_boundq_1_alt<int, sizeof(int)> *queue)
{
	int *bin = queue->write_prepare();
	*bin = 1;
	queue->write_publish();
}

void threadB(struct mpmc_boundq_1_alt<int, sizeof(int)> *queue)
{
	int *bin = queue->read_fetch();
	printf("Read: %d\n", *bin);
	queue->read_consume();
}

int user_main(int argc, char **argv)
{
	struct mpmc_boundq_1_alt<int, sizeof(int)> queue;
	thrd_t A, B;

	thrd_create(&A, (thrd_start_t)&threadA, &queue);
	thrd_create(&B, (thrd_start_t)&threadB, &queue);
	thrd_join(A);
	thrd_join(B);

	return 0;
}
