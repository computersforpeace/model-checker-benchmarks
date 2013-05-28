#include <stdatomic.h>
#include <inttypes.h>
#include "deque.h"
#include <stdlib.h>
#include <stdio.h>

Deque * create() {
	Deque * q = (Deque *) calloc(1, sizeof(Deque));
	Array * a = (Array *) calloc(1, sizeof(Array)+2*sizeof(atomic_int));
	atomic_store_explicit(&q->array, a, memory_order_relaxed);
	atomic_store_explicit(&q->top, 0, memory_order_relaxed);
	atomic_store_explicit(&q->bottom, 0, memory_order_relaxed);
	atomic_store_explicit(&a->size, 2, memory_order_relaxed);
	return q;
}

int take(Deque *q) {
	size_t b = atomic_load_explicit(&q->bottom, memory_order_relaxed) - 1;
	Array *a = (Array *) atomic_load_explicit(&q->array, memory_order_relaxed);
	atomic_store_explicit(&q->bottom, b, memory_order_relaxed);
	atomic_thread_fence(memory_order_seq_cst);
	size_t t = atomic_load_explicit(&q->top, memory_order_relaxed);
	int x;
	if (t <= b) {
		/* Non-empty queue. */
		x = atomic_load_explicit(&a->buffer[b % atomic_load_explicit(&a->size,memory_order_relaxed)], memory_order_relaxed);
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

void resize(Deque *q) {
	Array *a = (Array *) atomic_load_explicit(&q->array, memory_order_relaxed);
	size_t size=atomic_load_explicit(&a->size, memory_order_relaxed);
	size_t new_size=size << 1;
	Array *new_a = (Array *) calloc(1, new_size * sizeof(atomic_int) + sizeof(Array));
	size_t top=atomic_load_explicit(&q->top, memory_order_relaxed);
	size_t bottom=atomic_load_explicit(&q->bottom, memory_order_relaxed);
	atomic_store_explicit(&new_a->size, new_size, memory_order_relaxed);
	size_t i;
	for(i=top; i < bottom; i++) {
		atomic_store_explicit(&new_a->buffer[i % new_size], atomic_load_explicit(&a->buffer[i % size], memory_order_relaxed), memory_order_relaxed);
	}
	atomic_store_explicit(&q->array, new_a, memory_order_release);
	printf("resize\n");
}

void push(Deque *q, int x) {
	size_t b = atomic_load_explicit(&q->bottom, memory_order_relaxed);
	size_t t = atomic_load_explicit(&q->top, memory_order_acquire);
	Array *a = (Array *) atomic_load_explicit(&q->array, memory_order_relaxed);
	if (b - t > atomic_load_explicit(&a->size, memory_order_relaxed) - 1) /* Full queue. */ {
		resize(q);
		//Bug in paper...should have next line...
		a = (Array *) atomic_load_explicit(&q->array, memory_order_relaxed);
	}
	atomic_store_explicit(&a->buffer[b % atomic_load_explicit(&a->size, memory_order_relaxed)], x, memory_order_relaxed);
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
		Array *a = (Array *) atomic_load_explicit(&q->array, memory_order_acquire);
		x = atomic_load_explicit(&a->buffer[t % atomic_load_explicit(&a->size, memory_order_relaxed)], memory_order_relaxed);
		if (!atomic_compare_exchange_strong_explicit(&q->top, &t, t + 1, memory_order_seq_cst, memory_order_relaxed))
			/* Failed race. */
			return ABORT;
	}
	return x;
}
