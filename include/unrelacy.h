#include <stdatomic.h>

#define $

/* Should re-define to something meaningful */
#define ASSERT(expr)

#define mo_seqcst memory_order_relaxed
#define mo_release memory_order_release
#define mo_acquire memory_order_acquire
#define mo_acq_rel memory_order_acq_rel
#define mo_relaxed memory_order_relaxed

namespace rl {

	class backoff_t
	{
	 public:
		typedef int debug_info_param;
		void yield(debug_info_param info) { }
		void yield() { }
	};


	typedef backoff_t backoff;
	typedef backoff_t linear_backoff;
	typedef backoff_t exp_backoff;

}
