#include <inttypes.h>
#include <threads.h>
#include <stdio.h>

#include <librace.h>

#include "mpmc-queue.h"

void threadA(struct mpmc_boundq_1_alt<int32_t, sizeof(int32_t)> *queue)
{
	int32_t *bin = queue->write_prepare();
	store_32(bin, 1);
	queue->write_publish();
}

void threadB(struct mpmc_boundq_1_alt<int32_t, sizeof(int32_t)> *queue)
{
	int32_t *bin = queue->read_fetch();
	printf("Read: %d\n", load_32(bin));
	queue->read_consume();
}

int user_main(int argc, char **argv)
{
	struct mpmc_boundq_1_alt<int32_t, sizeof(int32_t)> queue;
	thrd_t A, B;

	int32_t *bin = queue.write_prepare();
	store_32(bin, 17);
	queue.write_publish();

	printf("Start threads\n");

	thrd_create(&A, (thrd_start_t)&threadA, &queue);
	thrd_create(&B, (thrd_start_t)&threadB, &queue);
	thrd_join(A);
	thrd_join(B);

	printf("Threads complete\n");

	return 0;
}
