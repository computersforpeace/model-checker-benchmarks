#include <stdatomic.h>

#define TRUE				1
#define FALSE				0

#define MAX_NODES			0xf
#define MAX_SERIAL			10000

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
	unsigned int foo[30];
} node_t;

typedef struct private {
	unsigned int node;
	unsigned int value;
	unsigned int serial[MAX_SERIAL];
} private_t;

typedef struct shared_mem {
	pointer_t head;
	unsigned int foo1[31];
	pointer_t tail;
	unsigned int foo2[31];
	node_t nodes[MAX_NODES+1];
	unsigned int serial;
} shared_mem_t;

void init_private(int pid);
void init_memory();
void init_queue();
unsigned int dequeue();
