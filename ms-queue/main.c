#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <threads.h>

#include "my_queue.h"

static int procs = 2;
static int iterations = 1;
private_t private;
static queue_t *queue;

static void parse_args(int argc, char **argv)
{
	extern char *optarg;
	int c;

	while ((c = getopt(argc, argv, "i:p:")) != EOF) {
		switch (c) {
		case 'i':
			iterations = atoi(optarg);
			break;
		case 'p':
			procs = atoi(optarg);
			break;
		default:
			assert(0);
		}
	}
}

static void main_task(void *param)
{
	unsigned i, j;
	unsigned val;
	int pid = *((int *)param);

	init_private(pid);
	for (i = 0; i < iterations; i++) {
		val = 1 + pid * iterations + i;
		enqueue(queue, val);

		val = dequeue(queue);
	}
}

int user_main(int argc, char **argv)
{
	int i, num_threads;
	thrd_t *t;
	int *param;

	parse_args(argc, argv);
	iterations = (iterations + (procs >> 1)) / procs;

	queue = calloc(1, sizeof(*queue));
	assert(queue);

	num_threads = procs;
	t = malloc(num_threads * sizeof(thrd_t));
	param = malloc(num_threads * sizeof(*param));

	init_queue(queue);
	for (i = 0; i < num_threads; i++) {
		param[i] = i;
		thrd_create(&t[i], main_task, &param[i]);
	}
	for (i = 0; i < num_threads; i++)
		thrd_join(t[i]);

	free(param);
	free(t);
	free(queue);

	return 0;
}
