#include <threads.h>
#include <stdlib.h>
#include "librace.h"

#include "my_queue.h"

#define relaxed memory_order_relaxed
#define release memory_order_release
#define acquire memory_order_acquire

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

	/* Note: needed to add this init manually */
	atomic_init(&q->nodes[0].next, 0);

	/* initialize queue */
	head = MAKE_POINTER(1, 0);
	tail = MAKE_POINTER(1, 0);
	next = MAKE_POINTER(0, 0); // (NULL, 0)

	atomic_init(&q->head, head);
	atomic_init(&q->tail, tail);
	atomic_init(&q->nodes[1].next, next);

	/* initialize avail list */
	for (i = 2; i < MAX_NODES; i++) {
		next = MAKE_POINTER(i + 1, 0);
		atomic_init(&q->nodes[i].next, next);
	}

	next = MAKE_POINTER(0, 0); // (NULL, 0)
	atomic_init(&q->nodes[MAX_NODES].next, next);
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
	tmp = atomic_load_explicit(&q->nodes[node].next, relaxed);
	set_ptr(&tmp, 0); // NULL
	atomic_store_explicit(&q->nodes[node].next, tmp, relaxed);

	while (!success) {
		tail = atomic_load_explicit(&q->tail, acquire);
		next = atomic_load_explicit(&q->nodes[get_ptr(tail)].next, acquire);
		if (tail == atomic_load_explicit(&q->tail, relaxed)) {
			if (get_ptr(next) == 0) { // == NULL
				pointer value = MAKE_POINTER(node, get_count(next) + 1);
				success = atomic_compare_exchange_strong_explicit(&q->nodes[get_ptr(tail)].next,
						&next, value, memory_order_acq_rel, memory_order_acq_rel);
			}
			if (!success) {
				unsigned int ptr = get_ptr(atomic_load_explicit(&q->nodes[get_ptr(tail)].next, memory_order_seq_cst));
				pointer value = MAKE_POINTER(ptr,
						get_count(tail) + 1);
				atomic_compare_exchange_strong_explicit(&q->tail,
						&tail, value,
						memory_order_acq_rel, memory_order_acq_rel);
				thrd_yield();
			}
		}
	}
	atomic_compare_exchange_strong_explicit(&q->tail,
			&tail,
			MAKE_POINTER(node, get_count(tail) + 1),
			memory_order_acq_rel, memory_order_acq_rel);
}

unsigned int dequeue(queue_t *q)
{
	unsigned int value;
	int success = 0;
	pointer head;
	pointer tail;
	pointer next;

	while (!success) {
		head = atomic_load_explicit(&q->head, acquire);
		tail = atomic_load_explicit(&q->tail, acquire);
		next = atomic_load_explicit(&q->nodes[get_ptr(head)].next, acquire);
		if (atomic_load_explicit(&q->head, relaxed) == head) {
			if (get_ptr(head) == get_ptr(tail)) {
				if (get_ptr(next) == 0) { // NULL
					return 0; // NULL
				}
				atomic_compare_exchange_strong_explicit(&q->tail,
						&tail,
						MAKE_POINTER(get_ptr(next), get_count(tail) + 1),
						memory_order_acq_rel, memory_order_acq_rel);
				thrd_yield();
			} else {
				value = load_32(&q->nodes[get_ptr(next)].value);
				success = atomic_compare_exchange_strong_explicit(&q->head,
						&head,
						MAKE_POINTER(get_ptr(next), get_count(head) + 1),
						memory_order_acq_rel, memory_order_acq_rel);
				if (!success)
					thrd_yield();
			}
		}
	}
	reclaim(get_ptr(head));
	return value;
}
