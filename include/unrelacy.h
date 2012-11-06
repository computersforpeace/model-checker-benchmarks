#ifndef __UNRELACY_H__
#define __UNRELACY_H__

#include <stdatomic.h>
#include <stdlib.h>
#include <stdio.h>
#include <mutex>
#include <condition_variable>

#include <model-assert.h>

#define $

#define ASSERT(expr) MODEL_ASSERT(expr)
#define RL_ASSERT(expr) MODEL_ASSERT(expr)

#define RL_NEW new
#define RL_DELETE(expr) delete expr

#define mo_seqcst memory_order_relaxed
#define mo_release memory_order_release
#define mo_acquire memory_order_acquire
#define mo_acq_rel memory_order_acq_rel
#define mo_relaxed memory_order_relaxed

namespace rl {

	template <typename T>
	struct var {
		var() { value = 0; }
		var(T v) { value = v; }
		var(var const& r) { value = r.value; }
		~var() { }

		void operator = (T v) { value = v; }
		T operator () () { return value; }
		void operator += (T v) { value += v; }
		bool operator == (const struct var<T> v) const { return value == v.value; }

		T value;
	};

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

#endif /* __UNRELACY_H__ */
