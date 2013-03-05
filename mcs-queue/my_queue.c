#include "main.h"

extern unsigned pid;
extern unsigned iterations;
extern unsigned initial_nodes;
extern unsigned backoff;
extern unsigned backoff_base;
extern private_t private;
extern shared_mem_t* smp;

void
init_private()
{
  private.node = 2 + initial_nodes + pid;
  private.value = 1 + initial_nodes + (pid * iterations);

}

void
init_memory()
{
}

unsigned
new_node()
{
  return private.node;
}

void
reclaim(unsigned node)
{
  private.node = node;
}

void
init_queue()
{
  unsigned i;

  /* initialize queue */
  smp->head.sep.ptr = 1;
  smp->head.sep.count = 0;
  smp->tail.sep.ptr = 1;
  smp->tail.sep.count = 0;
  smp->nodes[1].next.sep.ptr = NULL;
  smp->nodes[1].next.sep.count = 0;
  
  /* initialize avail list */
  for (i=2; i<MAX_NODES; i++) {
    smp->nodes[i].next.sep.ptr = i+1;
    smp->nodes[i].next.sep.count = 0;
  }
  smp->nodes[MAX_NODES].next.sep.ptr = NULL;
  smp->nodes[MAX_NODES].next.sep.count = 0;
  
  /* initialize queue contents */
  if (initial_nodes > 0) {
    for (i=2; i<initial_nodes+2; i++) {
      smp->nodes[i].value = i;
      smp->nodes[i-1].next.sep.ptr = i;
      smp->nodes[i].next.sep.ptr = NULL;
    }
    smp->head.sep.ptr = 1;
    smp->tail.sep.ptr = 1 + initial_nodes;    
  }
}

void
enqueue(unsigned val)
{
  unsigned success;
  unsigned node;
  pointer_t tail;
  pointer_t next;

  node = new_node();
  smp->nodes[node].value = val;
  smp->nodes[node].next.sep.ptr = NULL;

  backoff = backoff_base;
  for (success = FALSE; success == FALSE; ) {
    tail.con = smp->tail.con;
    next.con = smp->nodes[tail.sep.ptr].next.con;
    if (tail.con == smp->tail.con) {
      if (next.sep.ptr == NULL) {
	backoff = backoff_base;
	success = cas(&smp->nodes[tail.sep.ptr].next, 
		      next.con,
		      MAKE_LONG(node, next.sep.count+1));
      }
      if (success == FALSE) {
	cas(&smp->tail,
	    tail.con,
	    MAKE_LONG(smp->nodes[tail.sep.ptr].next.sep.ptr,
		      tail.sep.count+1));
	backoff_delay();
      }
    }
  }
  cas(&smp->tail, 
      tail.con,
      MAKE_LONG(node, tail.sep.count+1));
}

unsigned
dequeue()
{
  unsigned value;
  unsigned success;
  pointer_t head;
  pointer_t tail;
  pointer_t next;

  backoff = backoff_base;
  for (success = FALSE; success == FALSE; ) {
    head.con = smp->head.con;
    tail.con = smp->tail.con;
    next.con = smp->nodes[head.sep.ptr].next.con;
    if (smp->head.con == head.con) {
      if (head.sep.ptr == tail.sep.ptr) {
	if (next.sep.ptr == NULL) {
	  return NULL;
	}
	cas(&smp->tail,
	    tail.con,
	    MAKE_LONG(next.sep.ptr, tail.sep.count+1));
	backoff_delay();
      } else {
	value = smp->nodes[next.sep.ptr].value;
	success = cas(&smp->head,
		      head.con,
		      MAKE_LONG(next.sep.ptr, head.sep.count+1));
	if (success == FALSE) {
	  backoff_delay();
	}
      }
    }
  }
  reclaim(head.sep.ptr);
  return value;
}

