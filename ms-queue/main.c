#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <threads.h>

#include "my_queue.h"

static int procs = 2;
static int iterations = 1;
static queue_t *queue;
static thrd_t *threads;
static int num_threads;

int get_thread_num()
{
	thrd_t curr = thrd_current();
	int i;
	for (i = 0; i < num_threads; i++)
		if (curr.priv == threads[i].priv)
			return i;
	assert(0);
	return -1;
}

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
	unsigned int i, j;
	unsigned int val;
	int pid = *((int *)param);

	for (i = 0; i < iterations; i++) {
		val = 1 + pid * iterations + i;
		printf("worker %d, enqueueing: %u\n", pid, val);
		enqueue(queue, val);

		val = dequeue(queue);
		printf("worker %d, dequeued: %u\n", pid, val);
	}
}

int user_main(int argc, char **argv)
{
	int i;
	int *param;

	parse_args(argc, argv);

	queue = calloc(1, sizeof(*queue));
	assert(queue);

	num_threads = procs;
	threads = malloc(num_threads * sizeof(thrd_t));
	param = malloc(num_threads * sizeof(*param));

	init_queue(queue, num_threads);
	for (i = 0; i < num_threads; i++) {
		param[i] = i;
		thrd_create(&threads[i], main_task, &param[i]);
	}
	for (i = 0; i < num_threads; i++)
		thrd_join(threads[i]);

	free(param);
	free(threads);
	free(queue);

	return 0;
}
