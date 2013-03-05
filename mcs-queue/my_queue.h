#define TRUE				1
#define FALSE				0
#define NULL				0

#define MAX_NODES			0xff
#define MAX_SERIAL			10000

#define MAKE_LONG(lo, hi)		((hi)<<16)+(lo)

typedef union pointer {
  struct {
    volatile unsigned short count;
    volatile unsigned short ptr;
  } sep;
  volatile unsigned long con;
}pointer_t;

typedef struct node {
  unsigned value;
  pointer_t next;
  unsigned foo[30];
} node_t;

typedef struct private {
  unsigned node;
  unsigned value;
  unsigned serial[MAX_SERIAL];
} private_t;

typedef struct shared_mem {
  pointer_t head;
  unsigned foo1[31];
  pointer_t tail;
  unsigned foo2[31];
  node_t nodes[MAX_NODES+1];
  unsigned serial;
} shared_mem_t;

