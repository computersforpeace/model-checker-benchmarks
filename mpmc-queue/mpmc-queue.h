#include <stdatomic.h>
#include <unrelacy.h>

template <typename t_element, size_t t_size>
struct mpmc_boundq_1_alt
{
private:

	// elements should generally be cache-line-size padded :
	t_element               m_array[t_size];

	// rdwr counts the reads & writes that have started
	atomic<unsigned int>    m_rdwr;
	// "read" and "written" count the number completed
	atomic<unsigned int>    m_read;
	atomic<unsigned int>    m_written;

public:

	mpmc_boundq_1_alt()
	{
		m_rdwr = 0;
		m_read = 0;
		m_written = 0;
	}

	//-----------------------------------------------------

	t_element * read_fetch() {
		unsigned int rdwr = m_rdwr.load(mo_acquire);
		unsigned int rd,wr;
		for(;;) {
			rd = (rdwr>>16) & 0xFFFF;
			wr = rdwr & 0xFFFF;

			if ( wr == rd ) // empty
				return false;

			if ( m_rdwr.compare_exchange_weak(rdwr,rdwr+(1<<16),mo_acq_rel) )
				break;
			else
				thrd_yield();
		}

		// (*1)
		rl::backoff bo;
		while ( (m_written.load(mo_acquire) & 0xFFFF) != wr ) {
			thrd_yield();
		}

		t_element * p = & ( m_array[ rd % t_size ] );

		return p;
	}

	void read_consume() {
		m_read.fetch_add(1,mo_release);
	}

	//-----------------------------------------------------

	t_element * write_prepare() {
		unsigned int rdwr = m_rdwr.load(mo_acquire);
		unsigned int rd,wr;
		for(;;) {
			rd = (rdwr>>16) & 0xFFFF;
			wr = rdwr & 0xFFFF;

			if ( wr == ((rd + t_size)&0xFFFF) ) // full
				return NULL;

			if ( m_rdwr.compare_exchange_weak(rdwr,(rd<<16) | ((wr+1)&0xFFFF),mo_acq_rel) )
				break;
			else
				thrd_yield();
		}

		// (*1)
		rl::backoff bo;
		while ( (m_read.load(mo_acquire) & 0xFFFF) != rd ) {
			thrd_yield();
		}

		t_element * p = & ( m_array[ wr % t_size ] );

		return p;
	}

	void write_publish()
	{
		m_written.fetch_add(1,mo_release);
	}

	//-----------------------------------------------------


};
