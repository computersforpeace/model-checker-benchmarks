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
	rl::test_params params;
	params.search_type = rl::fair_full_search_scheduler_type;
	rl::simulate<spsc_queue_test>(params);
}
