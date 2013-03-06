#include "main.h"
#include <stdlib.h>

unsigned procs = 2;
unsigned iterations = 1;
private_t private;
shared_mem_t *smp;

static void main_task(void *param)
{
	unsigned i, j;
	unsigned val;
	int pid = *((int *)param);

	init_memory();
	init_private(pid);
	for (i = 0; i < iterations; i++) {
		val = private.value;
		enqueue(val);
		val = dequeue();
		private.value++;
	}
}

int user_main(int argc, char **argv)
{
	int i, num_threads;
	thrd_t *t;
	int *param;

	parse_args(argc, argv);
	iterations = (iterations + (procs >> 1)) / procs;

	smp = (shared_mem_t *)calloc(1, sizeof(shared_mem_t));
	assert(smp);

	num_threads = procs;
	t = malloc(num_threads * sizeof(thrd_t));
	param = malloc(num_threads * sizeof(*param));

	init_queue();
	for (i = 0; i < num_threads; i++) {
		param[i] = i;
		thrd_create(&t[i], main_task, &param[i]);
	}
	for (i = 0; i < num_threads; i++)
		thrd_join(t[i]);

	free(param);
	free(t);
	free(smp);

	return 0;
}
