#include "main.h"

#define NUM_PROCESSORS			12

struct tms tim;
struct tms tim1;

usptr_t *Handle;
barrier_t *Barrier;
usptr_t *lock_handle;
ulock_t native_lock;

int shmid;

unsigned pid;
unsigned backoff;
unsigned backoff_base;
unsigned backoff_cap;
unsigned backoff_addend;
char* name = "";
unsigned backoff_base_bits = 0;
unsigned backoff_cap_bits = 0;
unsigned procs = 1;
unsigned multi = 1;
unsigned iterations = 1;
unsigned initial_nodes = 0;
unsigned repetitions = 1;
unsigned backoff_shift_bits = 0;
unsigned work = 0;
private_t private;
shared_mem_t *smp;

void
native_acquire ()
{
  ussetlock(native_lock);
} 

void
native_release ()
{
  usunsetlock(native_lock);
}

void
tts_acq(unsigned* plock)
{
  do {
    if (*plock == 1) {
      backoff = backoff_base;
      do {
	backoff_delay();
      } while(*plock == 1);
    }
  } while (tas(plock) == 1);
}

void 
time_test ()
{
  unsigned i,j;
  struct tms time_val;
  clock_t t1, t2;
  unsigned val;

  if(pid==0) {
    init_queue();
  }
  init_memory();
  init_private();
  init_backoff();
  barrier(Barrier, procs*multi);
  if(pid==0)
  {
    t1=times(&time_val);
  }
  for(i=0;i<iterations;i++) {
    val = private.value;
    enqueue(val);
    for(j=0; j<work;) j++;
    val = dequeue();
    for(j=0; j<work;) j++;
    private.value++;
  }
  barrier(Barrier, procs*multi);
  if(pid==0)
  {
    t2=times(&time_val);
    printf("p%d  m%d  i%d  b%d c%d s%d  w%d time %.0f ms.\n",
	   procs, multi, iterations*procs*multi,
	   backoff_base_bits, backoff_cap_bits,
	   backoff_shift_bits, work, ((t2-t1)*1000)/(double)HZ);
    fflush(stdout);
  }
}

void main_task()
{
  unsigned processor;
  unsigned i;

  processor = (pid/multi)+1;
  processor %= NUM_PROCESSORS;
  if(sysmp(MP_MUSTRUN, processor) == -1) { perror("Could not MUSTRUN"); }
  if (pid==0) {
    printf("--- %s\n", name);
    fflush(stdout);
  }
  for (i=0; i<repetitions; i++) {
    time_test();
  }
}

void  setup_shmem()
{
  shmid = shmget(IPC_PRIVATE, sizeof(shared_mem_t), 511);
  assert(shmid != -1);
  smp = (shared_mem_t *)shmat(shmid, 0, 0);
  assert((int)smp != -1);
}

void my_m_fork(void (*func)(),int num_procs)
{
  for (pid=1;pid<num_procs;pid++) {
    if(fork()==0) /* Child */ {
      (*func)(); /* Call the program */
      return;
    }
  }
  pid=0;
  (*func)(); /* Call the program */
}

main(int argc,char **argv)
{
  parse_args(argc, argv);
  name = argv[0];
  iterations = (iterations + ((procs*multi)>>1))/(procs*multi);
  setup_shmem();
  Handle = usinit("/tmp/foo_barrier");
  Barrier = new_barrier(Handle);
  init_barrier(Barrier);
  lock_handle = usinit("/tmp/foo_lock");
  native_lock = usnewlock(lock_handle);
  my_m_fork(main_task, procs*multi); 
  shmctl(shmid, IPC_RMID, smp);
  exit(0);
}
