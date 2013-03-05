#include "main.h"

extern unsigned iterations;
extern unsigned multi;
extern unsigned initial_nodes;
extern unsigned procs;
extern unsigned repetitions;
extern unsigned work;

void parse_args(int argc, char **argv)
{
	extern char * optarg;
	int c;

	while ((c = getopt(argc, argv, "i:m:n:p:r:w:")) != EOF)
		switch(c) {
			case 'i':  iterations = atoi(optarg); break;
			case 'm':  multi = atoi(optarg);  break;
			case 'n':  initial_nodes = atoi(optarg);  break;
			case 'p':   procs = atoi(optarg);   break;
			case 'r':   repetitions = atoi(optarg);   break;
			case 'w':   work = atoi(optarg);   break;
			default:
				    assert(0);
		}
}
