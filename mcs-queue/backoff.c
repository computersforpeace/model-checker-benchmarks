extern unsigned backoff;
extern unsigned backoff_base_bits;
extern unsigned backoff_cap_bits;
extern unsigned backoff_shift_bits;
extern unsigned backoff_base;
extern unsigned backoff_cap;
extern unsigned backoff_addend;

void
init_backoff()
{
  backoff_base = (1<<backoff_base_bits)-1;
  backoff_cap = (1<<backoff_cap_bits)-1;
  backoff_addend = (1<<backoff_shift_bits)-1;
}

unsigned
backoff_delay()
{
  unsigned i;
  
  for (i=0; i<backoff; i++) ;
  backoff <<= backoff_shift_bits;
  backoff += backoff_addend;
  backoff &= backoff_cap;
  return i;
}
