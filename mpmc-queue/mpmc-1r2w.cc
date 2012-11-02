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

#define MAXREADERS 3
#define MAXWRITERS 3

int readers = 1, writers = 2;

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
	thrd_t A[MAXWRITERS], B[MAXREADERS];

	/* Note: optarg() / optind is broken in model-checker - workaround is
	 * to just copy&paste this test a few times */
	//process_params(argc, argv);
	printf("%d reader(s), %d writer(s)\n", readers, writers);

	int32_t *bin = queue.write_prepare();
	store_32(bin, 17);
	queue.write_publish();

	printf("Start threads\n");

	for (int i = 0; i < writers; i++)
		thrd_create(&A[i], (thrd_start_t)&threadA, &queue);
	for (int i = 0; i < readers; i++)
		thrd_create(&B[i], (thrd_start_t)&threadB, &queue);

	for (int i = 0; i < writers; i++)
		thrd_join(A[i]);
	for (int i = 0; i < readers; i++)
		thrd_join(B[i]);

	printf("Threads complete\n");

	return 0;
}
