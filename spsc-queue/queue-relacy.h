#include "eventcount-relacy.h"

template<typename T>
class spsc_queue
{
public:
	spsc_queue()
	{
		node* n = new node ();
		head($) = n;
		tail($) = n;
	}

	~spsc_queue()
	{
		RL_ASSERT(head($) == tail($));
		delete ((node*)head($));
	}

	void enqueue(T data)
	{
		node* n = new node (data);
		head($)->next.store(n, std::memory_order_release);
		head($) = n;
		ec.signal_relaxed();
	}

	T dequeue()
	{
		T data = try_dequeue();
		while (0 == data)
		{
			int cmp = ec.get();
			data = try_dequeue();
			if (data)
				break;
			ec.wait(cmp);
			data = try_dequeue();
			if (data)
				break;
		}
		return data;
	}

private:
	struct node
	{
		std::atomic<node*> next;
		rl::var<T> data;

		node(T data = T())
			: data(data)
		{
			next($) = 0;
		}
	};

	rl::var<node*> head;
	rl::var<node*> tail;

	eventcount ec;

	T try_dequeue()
	{
		node* t = tail($);
		node* n = t->next.load(std::memory_order_acquire);
		if (0 == n)
			return 0;
		T data = n->data($);
		delete (t);
		tail($) = n;
		return data;
	}
};
