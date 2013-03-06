#include "main.h"

extern unsigned int iterations;
extern private_t private;
extern shared_mem_t *smp;

void init_private(int pid)
{
	private.node = 2 + pid;
	private.value = 1 + (pid * iterations);

}

void init_memory()
{
}

static unsigned int new_node()
{
	return private.node;
}

static void reclaim(unsigned int node)
{
	private.node = node;
}

void init_queue()
{
	unsigned int i;
	pointer head;
	pointer tail;
	pointer next;

	/* initialize queue */
	head = MAKE_POINTER(1, 0);
	tail = MAKE_POINTER(1, 0);
	next = MAKE_POINTER(0, 0); // (NULL, 0)

	atomic_init(&smp->nodes[0].next, 0); // assumed inititalized in original example

	atomic_store(&smp->head, head);
	atomic_store(&smp->tail, tail);
	atomic_store(&smp->nodes[1].next, next);

	/* initialize avail list */
	for (i = 2; i < MAX_NODES; i++) {
		next = MAKE_POINTER(i + 1, 0);
		atomic_store(&smp->nodes[i].next, next);
	}

	next = MAKE_POINTER(0, 0); // (NULL, 0)
	atomic_store(&smp->nodes[MAX_NODES].next, next);
}

void enqueue(unsigned int val)
{
	unsigned int success = 0;
	unsigned int node;
	pointer tail;
	pointer next;
	pointer tmp;

	node = new_node();
	smp->nodes[node].value = val;
	tmp = atomic_load(&smp->nodes[node].next);
	set_ptr(&tmp, 0); // NULL
	atomic_store(&smp->nodes[node].next, tmp);

	while (!success) {
		tail = atomic_load(&smp->tail);
		next = atomic_load(&smp->nodes[get_ptr(tail)].next);
		if (tail == atomic_load(&smp->tail)) {
			if (get_ptr(next) == 0) { // == NULL
				pointer val = MAKE_POINTER(node, get_count(next) + 1);
				success = atomic_compare_exchange_weak(&smp->nodes[get_ptr(tail)].next,
						&next,
						val);
			}
			if (!success) {
				unsigned int ptr = get_ptr(atomic_load(&smp->nodes[get_ptr(tail)].next));
				pointer val = MAKE_POINTER(ptr,
						get_count(tail) + 1);
				atomic_compare_exchange_strong(&smp->tail,
						&tail,
						val);
				thrd_yield();
			}
		}
	}
	atomic_compare_exchange_strong(&smp->tail,
			&tail,
			MAKE_POINTER(node, get_count(tail) + 1));
}

unsigned int dequeue()
{
	unsigned int value;
	unsigned int success;
	pointer head;
	pointer tail;
	pointer next;

	for (success = FALSE; success == FALSE; ) {
		head = atomic_load(&smp->head);
		tail = atomic_load(&smp->tail);
		next = atomic_load(&smp->nodes[get_ptr(head)].next);
		if (atomic_load(&smp->head) == head) {
			if (get_ptr(head) == get_ptr(tail)) {
				if (get_ptr(next) == 0) { // NULL
					return 0; // NULL
				}
				atomic_compare_exchange_weak(&smp->tail,
						&tail,
						MAKE_POINTER(get_ptr(next), get_count(tail) + 1));
				thrd_yield();
			} else {
				value = smp->nodes[get_ptr(next)].value;
				success = atomic_compare_exchange_weak(&smp->head,
						&head,
						MAKE_POINTER(get_ptr(next), get_count(head) + 1));
				if (success == FALSE) {
					thrd_yield();
				}
			}
		}
	}
	reclaim(get_ptr(head));
	return value;
}
