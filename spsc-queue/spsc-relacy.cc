#include <relacy/relacy_std.hpp>

#include "queue-relacy.h"

struct spsc_queue_test : rl::test_suite<spsc_queue_test, 2>
{
	spsc_queue<int> q;

	void thread(unsigned thread_index)
	{
		if (0 == thread_index)
		{
			q.enqueue(11);
		}
		else
		{
			int d = q.dequeue();
			RL_ASSERT(11 == d);
		}
	}
};


int main()
{
	rl::simulate<spsc_queue_test>();
}
