#include "main.h"

extern unsigned backoff_base_bits;
extern unsigned backoff_cap_bits;
extern unsigned iterations;
extern unsigned multi;
extern unsigned initial_nodes;
extern unsigned procs;
extern unsigned repetitions;
extern unsigned backoff_shift_bits;
extern unsigned work;

void 
parse_args(int argc,char **argv)
{
extern char * optarg; 
int c; 

  while((c=getopt(argc,argv,"b:c:i:m:n:p:r:s:w:"))!=EOF)
    switch(c){
    case 'b':  backoff_base_bits = atoi(optarg); break;
    case 'c':  backoff_cap_bits = atoi(optarg); break;
    case 'i':  iterations = atoi(optarg); break;
    case 'm':  multi = atoi(optarg);  break;
    case 'n':  initial_nodes = atoi(optarg);  break;
    case 'p':   procs = atoi(optarg);   break;
    case 'r':   repetitions = atoi(optarg);   break;
    case 's':   backoff_shift_bits = atoi(optarg);   break;
    case 'w':   work = atoi(optarg);   break;
    default: 
      assert(0);
    }
}
