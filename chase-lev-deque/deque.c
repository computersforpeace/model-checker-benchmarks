typedef struct {
	atomic_size_t size;
	atomic_int buffer[];
} Array;

typedef struct {
	atomic_size_t top, bottom;
	Atomic(Array *) array;
} Deque;

int take(Deque *q) {
	size_t b = load_explicit(&q->bottom, relaxed) - 1;
	Array *a = load_explicit(&q->array, relaxed);
	store_explicit(&q->bottom, b, relaxed);
	thread_fence(seq_cst);
	size_t t = load_explicit(&q->top, relaxed);
	int x;
	if (t <= b) {
		/* Non-empty queue. */
		x = load_explicit(&a->buffer[b % a->size], relaxed);
		if (t == b) {
			/* Single last element in queue. */
			if (!compare_exchange_strong_explicit(&q->top, &t, t + 1, seq_cst, relaxed))
				/* Failed race. */
				x = EMPTY;
			store_explicit(&q->bottom, b + 1, relaxed);
		}
	} else { /* Empty queue. */
		x = EMPTY;
		store_explicit(&q->bottom, b + 1, relaxed);
	}
	return x;
}

void push(Deque *q, int x) {
	size_t b = load_explicit(&q->bottom, relaxed);
	size_t t = load_explicit(&q->top, acquire);
	Array *a = load_explicit(&q->array, relaxed);
	if (b - t > a->size - 1) /* Full queue. */
		resize(q);
	store_explicit(&a->buffer[b % a->size], x, relaxed);
	thread_fence(release);
	store_explicit(&q->bottom, b + 1, relaxed);
}

int steal(Deque *q) {
	size_t t = load_explicit(&q->top, acquire);
	thread_fence(seq_cst);
	size_t b = load_explicit(&q->bottom, acquire);
	int x = EMPTY;
	if (t < b) {
		/* Non-empty queue. */
		Array *a = load_explicit(&q->array, relaxed);
		x = load_explicit(&a->buffer[t % a->size], relaxed);
		if (!compare_exchange_strong_explicit(&q->top, &t, t + 1, seq_cst, relaxed))
			/* Failed race. */
			return ABORT;
	}
	return x;
}
