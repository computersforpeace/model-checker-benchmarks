#include <threads.h>
#include <stdlib.h>
#include "librace.h"

#include "my_queue.h"

static unsigned int *node_nums;

static unsigned int new_node()
{
	return node_nums[get_thread_num()];
}

static void reclaim(unsigned int node)
{
	node_nums[get_thread_num()] = node;
}

void init_queue(queue_t *q, int num_threads)
{
	unsigned int i;
	pointer head;
	pointer tail;
	pointer next;

	node_nums = malloc(num_threads * sizeof(*node_nums));
	for (i = 0; i < num_threads; i++)
		node_nums[i] = 2 + i;

	/* initialize queue */
	head = MAKE_POINTER(1, 0);
	tail = MAKE_POINTER(1, 0);
	next = MAKE_POINTER(0, 0); // (NULL, 0)

	atomic_store(&q->head, head);
	atomic_store(&q->tail, tail);
	atomic_store(&q->nodes[1].next, next);

	/* initialize avail list */
	for (i = 2; i < MAX_NODES; i++) {
		next = MAKE_POINTER(i + 1, 0);
		atomic_store(&q->nodes[i].next, next);
	}

	next = MAKE_POINTER(0, 0); // (NULL, 0)
	atomic_store(&q->nodes[MAX_NODES].next, next);
}

void enqueue(queue_t *q, unsigned int val)
{
	int success = 0;
	unsigned int node;
	pointer tail;
	pointer next;
	pointer tmp;

	node = new_node();
	store_32(&q->nodes[node].value, val);
	tmp = atomic_load(&q->nodes[node].next);
	set_ptr(&tmp, 0); // NULL
	atomic_store(&q->nodes[node].next, tmp);

	while (!success) {
		tail = atomic_load(&q->tail);
		next = atomic_load(&q->nodes[get_ptr(tail)].next);
		if (tail == atomic_load(&q->tail)) {
			if (get_ptr(next) == 0) { // == NULL
				pointer value = MAKE_POINTER(node, get_count(next) + 1);
				success = atomic_compare_exchange_weak(&q->nodes[get_ptr(tail)].next,
						&next, value);
			}
			if (!success) {
				unsigned int ptr = get_ptr(atomic_load(&q->nodes[get_ptr(tail)].next));
				pointer value = MAKE_POINTER(ptr,
						get_count(tail) + 1);
				atomic_compare_exchange_strong(&q->tail,
						&tail, value);
				thrd_yield();
			}
		}
	}
	atomic_compare_exchange_strong(&q->tail,
			&tail,
			MAKE_POINTER(node, get_count(tail) + 1));
}

unsigned int dequeue(queue_t *q)
{
	unsigned int value;
	int success = 0;
	pointer head;
	pointer tail;
	pointer next;

	while (!success) {
		head = atomic_load(&q->head);
		tail = atomic_load(&q->tail);
		next = atomic_load(&q->nodes[get_ptr(head)].next);
		if (atomic_load(&q->head) == head) {
			if (get_ptr(head) == get_ptr(tail)) {
				if (get_ptr(next) == 0) { // NULL
					return 0; // NULL
				}
				atomic_compare_exchange_strong(&q->tail,
						&tail,
						MAKE_POINTER(get_ptr(next), get_count(tail) + 1));
				thrd_yield();
			} else {
				value = load_32(&q->nodes[get_ptr(next)].value);
				success = atomic_compare_exchange_weak(&q->head,
						&head,
						MAKE_POINTER(get_ptr(next), get_count(head) + 1));
				if (!success)
					thrd_yield();
			}
		}
	}
	reclaim(get_ptr(head));
	return value;
}
