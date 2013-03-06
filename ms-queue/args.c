#include "main.h"

extern unsigned iterations;
extern unsigned procs;

void parse_args(int argc, char **argv)
{
	extern char *optarg;
	int c;

	while ((c = getopt(argc, argv, "i:p:")) != EOF)
		switch(c) {
			case 'i':  iterations = atoi(optarg); break;
			case 'p':   procs = atoi(optarg);   break;
			default:
				    assert(0);
		}
}
