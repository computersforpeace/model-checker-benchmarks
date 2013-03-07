#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <threads.h>
#include <stdatomic.h>
#include <stdio.h>

#include "model-assert.h"

#include "deque.h"

Deque *q;
int a;
int b;

static void task(void * param) {
	do {
		a=steal(q);
	} while(a==EMPTY);
}

int user_main(int argc, char **argv)
{
	thrd_t t;
	q=create();
	thrd_create(&t, task, 0);
	push(q, 1);
	push(q, 2);
	b=take(q);
	thrd_join(t);

	printf("a=%d b=%d\n",a,b);
	MODEL_ASSERT(a + b == 3);

	return 0;
}
