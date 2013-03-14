/*
 * Dekker's critical section algorithm, implemented with fences.
 *
 * URL:
 *   http://www.justsoftwaresolutions.co.uk/threading/
 */

#include <atomic>
#include <threads.h>

#include "librace.h"

std::atomic<bool> flag0, flag1;
std::atomic<int> turn;

uint32_t var = 0;

void p0(void *arg)
{
	flag0.store(true,std::memory_order_relaxed);
	std::atomic_thread_fence(std::memory_order_seq_cst);

	while (flag1.load(std::memory_order_relaxed))
	{
		if (turn.load(std::memory_order_relaxed) != 0)
		{
			flag0.store(false,std::memory_order_relaxed);
			while (turn.load(std::memory_order_relaxed) != 0)
			{
				thrd_yield();
			}
			flag0.store(true,std::memory_order_relaxed);
			std::atomic_thread_fence(std::memory_order_seq_cst);
		} else
			thrd_yield();
	}
	std::atomic_thread_fence(std::memory_order_acquire);

	// critical section
	store_32(&var, 1);

	turn.store(1,std::memory_order_relaxed);
	std::atomic_thread_fence(std::memory_order_release);
	flag0.store(false,std::memory_order_relaxed);
}

void p1(void *arg)
{
	flag1.store(true,std::memory_order_relaxed);
	std::atomic_thread_fence(std::memory_order_seq_cst);

	while (flag0.load(std::memory_order_relaxed))
	{
		if (turn.load(std::memory_order_relaxed) != 1)
		{
			flag1.store(false,std::memory_order_relaxed);
			while (turn.load(std::memory_order_relaxed) != 1)
			{
				thrd_yield();
			}
			flag1.store(true,std::memory_order_relaxed);
			std::atomic_thread_fence(std::memory_order_seq_cst);
		} else
			thrd_yield();
	}
	std::atomic_thread_fence(std::memory_order_acquire);

	// critical section
	store_32(&var, 2);

	turn.store(0,std::memory_order_relaxed);
	std::atomic_thread_fence(std::memory_order_release);
	flag1.store(false,std::memory_order_relaxed);
}

int user_main(int argc, char **argv)
{
	thrd_t a, b;

	flag0 = false;
	flag1 = false;
	turn = 0;

	thrd_create(&a, p0, NULL);
	thrd_create(&b, p1, NULL);

	thrd_join(a);
	thrd_join(b);

	return 0;
}
