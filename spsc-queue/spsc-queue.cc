#include <threads.h>

#include "queue.h"

spsc_queue<int> *q;

	void thread(unsigned thread_index)
	{
		if (0 == thread_index)
		{
			q->enqueue(11);
		}
		else
		{
			int d = q->dequeue();
			RL_ASSERT(11 == d);
		}
	}

int user_main(int argc, char **argv)
{
	thrd_t A, B;

	q = new spsc_queue<int>();

	thrd_create(&A, (thrd_start_t)&thread, (void *)0);
	thrd_create(&B, (thrd_start_t)&thread, (void *)1);
	thrd_join(A);
	thrd_join(B);

	delete q;

	return 0;
}
