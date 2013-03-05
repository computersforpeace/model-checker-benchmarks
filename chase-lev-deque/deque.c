#include <stdatomic.h>
#include <inttypes.h>

typedef struct {
	atomic_size_t size;
	atomic_int buffer[];
} Array;

typedef struct {
	atomic_size_t top, bottom;
	atomic_uintptr_t array; /* Atomic(Array *) */
} Deque;

int take(Deque *q) {
	size_t b = atomic_load_explicit(&q->bottom, memory_order_relaxed) - 1;
	Array *a = atomic_load_explicit(&q->array, memory_order_relaxed);
	atomic_store_explicit(&q->bottom, b, memory_order_relaxed);
	atomic_thread_fence(memory_order_seq_cst);
	size_t t = atomic_load_explicit(&q->top, memory_order_relaxed);
	int x;
	if (t <= b) {
		/* Non-empty queue. */
		x = atomic_load_explicit(&a->buffer[b % a->size], memory_order_relaxed);
		if (t == b) {
			/* Single last element in queue. */
			if (!atomic_compare_exchange_strong_explicit(&q->top, &t, t + 1, memory_order_seq_cst, memory_order_relaxed))
				/* Failed race. */
				x = EMPTY;
			atomic_store_explicit(&q->bottom, b + 1, memory_order_relaxed);
		}
	} else { /* Empty queue. */
		x = EMPTY;
		atomic_store_explicit(&q->bottom, b + 1, memory_order_relaxed);
	}
	return x;
}

void push(Deque *q, int x) {
	size_t b = atomic_load_explicit(&q->bottom, memory_order_relaxed);
	size_t t = atomic_load_explicit(&q->top, memory_order_acquire);
	Array *a = atomic_load_explicit(&q->array, memory_order_relaxed);
	if (b - t > a->size - 1) /* Full queue. */
		resize(q);
	atomic_store_explicit(&a->buffer[b % a->size], x, memory_order_relaxed);
	atomic_thread_fence(memory_order_release);
	atomic_store_explicit(&q->bottom, b + 1, memory_order_relaxed);
}

int steal(Deque *q) {
	size_t t = atomic_load_explicit(&q->top, memory_order_acquire);
	atomic_thread_fence(memory_order_seq_cst);
	size_t b = atomic_load_explicit(&q->bottom, memory_order_acquire);
	int x = EMPTY;
	if (t < b) {
		/* Non-empty queue. */
		Array *a = atomic_load_explicit(&q->array, memory_order_relaxed);
		x = atomic_load_explicit(&a->buffer[t % a->size], memory_order_relaxed);
		if (!atomic_compare_exchange_strong_explicit(&q->top, &t, t + 1, memory_order_seq_cst, memory_order_relaxed))
			/* Failed race. */
			return ABORT;
	}
	return x;
}
