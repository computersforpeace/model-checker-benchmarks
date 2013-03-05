#include "main.h"
#include <stdlib.h>

#define NUM_PROCESSORS			12

struct tms tim;
struct tms tim1;

int shmid;

unsigned pid;
char* name = "";
unsigned procs = 1;
unsigned multi = 1;
unsigned iterations = 1;
unsigned initial_nodes = 0;
unsigned repetitions = 1;
unsigned work = 0;
private_t private;
shared_mem_t *smp;

void time_test()
{
	unsigned i,j;
	struct tms time_val;
	clock_t t1, t2;
	unsigned val;

	if(pid==0) {
		init_queue();
	}
	init_memory();
	init_private();
	for(i=0;i<iterations;i++) {
		val = private.value;
		enqueue(val);
		for(j=0; j<work;) j++;
		val = dequeue();
		for(j=0; j<work;) j++;
		private.value++;
	}
}

void main_task()
{
	unsigned processor;
	unsigned i;

	processor = (pid/multi)+1;
	processor %= NUM_PROCESSORS;
	for (i=0; i<repetitions; i++) {
		time_test();
	}
}

int user_main(int argc, char **argv)
{
	int i, num_threads;
	thrd_t *t;

	parse_args(argc, argv);
	name = argv[0];
	iterations = (iterations + ((procs*multi)>>1))/(procs*multi);

	smp = (shared_mem_t *)calloc(1, sizeof(shared_mem_t));
	assert(smp);

	num_threads = procs * multi;
	t = malloc(num_threads * sizeof(thrd_t));

	for (i = 0; i < num_threads; i++)
		thrd_create(&t[i], main_task, NULL);
	for (i = 0; i < num_threads; i++)
		thrd_join(t[i]);

	free(t);
	free(smp);

	return 0;
}
