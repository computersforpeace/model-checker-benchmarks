//#include <threads.h>
#include <thread>

#include "williams-queue.h"

lock_free_queue<int> *queue;

void threadA(void *arg)
{
}

#define user_main main

int user_main(int argc, char **argv)
{
	/*thrd_t A, B;

	thrd_create(&A, &threadA, NULL);
	thrd_join(A);*/
	queue = new lock_free_queue<int>();
	std::thread t(threadA, (void *)NULL);
	t.join();

	return 0;
}
