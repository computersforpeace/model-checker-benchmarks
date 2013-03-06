#include <stdatomic.h>

#define MAX_NODES			0xf

typedef unsigned long long pointer;
typedef atomic_ullong pointer_t;

#define MAKE_POINTER(ptr, count)	((((pointer)count) << 32) | ptr)
#define PTR_MASK 0xffffffffLL
#define COUNT_MASK (0xffffffffLL << 32)

static inline void set_count(pointer *p, unsigned int val) { *p = (*p & ~COUNT_MASK) | ((pointer)val << 32); }
static inline void set_ptr(pointer *p, unsigned int val) { *p = (*p & ~PTR_MASK) | val; }
static inline unsigned int get_count(pointer p) { return p & PTR_MASK; }
static inline unsigned int get_ptr(pointer p) { return (p & COUNT_MASK) >> 32; }

typedef struct node {
	unsigned int value;
	pointer_t next;
} node_t;

typedef struct private {
	unsigned int node;
} private_t;

typedef struct shared_mem {
	pointer_t head;
	pointer_t tail;
	node_t nodes[MAX_NODES + 1];
} queue_t;

void init_private(int pid);
void init_queue(queue_t *q);
void enqueue(queue_t *q, unsigned int val);
unsigned int dequeue(queue_t *q);
