#include <stdio.h>
#include <threads.h>

#include "mcs-lock.h"

struct mcs_mutex mutex;

int user_main(int argc, char **argv)
{
	mcs_mutex::guard *g = new mcs_mutex::guard(&mutex);
	mutex.lock(g);
	mutex.unlock(g);
	return 0;
}
