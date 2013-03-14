#include <inttypes.h>
#include <threads.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

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
	int32_t *bin;
	while (bin = queue->read_fetch()) {
		printf("Read: %d\n", load_32(bin));
		queue->read_consume();
	}
}

void threadC(struct mpmc_boundq_1_alt<int32_t, sizeof(int32_t)> *queue)
{
	int32_t *bin = queue->write_prepare();
	store_32(bin, 1);
	queue->write_publish();

	while (bin = queue->read_fetch()) {
		printf("Read: %d\n", load_32(bin));
		queue->read_consume();
	}
}

#define MAXREADERS 3
#define MAXWRITERS 3
#define MAXRDWR 3

#ifdef CONFIG_MPMC_READERS
#define DEFAULT_READERS (CONFIG_MPMC_READERS)
#else
#define DEFAULT_READERS 2
#endif

#ifdef CONFIG_MPMC_WRITERS
#define DEFAULT_WRITERS (CONFIG_MPMC_WRITERS)
#else
#define DEFAULT_WRITERS 2
#endif

#ifdef CONFIG_MPMC_RDWR
#define DEFAULT_RDWR (CONFIG_MPMC_RDWR)
#else
#define DEFAULT_RDWR 0
#endif

int readers = DEFAULT_READERS, writers = DEFAULT_WRITERS, rdwr = DEFAULT_RDWR;

void print_usage()
{
	printf("Error: use the following options\n"
		" -r <num>              Choose number of reader threads\n"
		" -w <num>              Choose number of writer threads\n");
	exit(EXIT_FAILURE);
}

void process_params(int argc, char **argv)
{
	const char *shortopts = "hr:w:";
	int opt;
	bool error = false;

	while (!error && (opt = getopt(argc, argv, shortopts)) != -1) {
		switch (opt) {
		case 'h':
			print_usage();
			break;
		case 'r':
			readers = atoi(optarg);
			break;
		case 'w':
			writers = atoi(optarg);
			break;
		default: /* '?' */
			error = true;
			break;
		}
	}

	if (writers < 1 || writers > MAXWRITERS)
		error = true;
	if (readers < 1 || readers > MAXREADERS)
		error = true;

	if (error)
		print_usage();
}

int user_main(int argc, char **argv)
{
	struct mpmc_boundq_1_alt<int32_t, sizeof(int32_t)> queue;
	thrd_t A[MAXWRITERS], B[MAXREADERS], C[MAXRDWR];

	/* Note: optarg() / optind is broken in model-checker - workaround is
	 * to just copy&paste this test a few times */
	//process_params(argc, argv);
	printf("%d reader(s), %d writer(s)\n", readers, writers);

#ifndef CONFIG_MPMC_NO_INITIAL_ELEMENT
	printf("Adding initial element\n");
	int32_t *bin = queue.write_prepare();
	store_32(bin, 17);
	queue.write_publish();
#endif

	printf("Start threads\n");

	for (int i = 0; i < writers; i++)
		thrd_create(&A[i], (thrd_start_t)&threadA, &queue);
	for (int i = 0; i < readers; i++)
		thrd_create(&B[i], (thrd_start_t)&threadB, &queue);

	for (int i = 0; i < rdwr; i++)
		thrd_create(&C[i], (thrd_start_t)&threadC, &queue);

	for (int i = 0; i < writers; i++)
		thrd_join(A[i]);
	for (int i = 0; i < readers; i++)
		thrd_join(B[i]);
	for (int i = 0; i < rdwr; i++)
		thrd_join(C[i]);

	printf("Threads complete\n");

	return 0;
}
