include ../benchmarks.mk

TESTNAME = mpmc-queue
TESTS = mpmc-queue mpmc-1r2w mpmc-2r1w

all: $(TESTS)

mpmc-queue: CPPFLAGS += -DCONFIG_MPMC_READERS=2 -DCONFIG_MPMC_WRITERS=2
mpmc-1r2w: CPPFLAGS += -DCONFIG_MPMC_READERS=1 -DCONFIG_MPMC_WRITERS=2
mpmc-2r1w: CPPFLAGS += -DCONFIG_MPMC_READERS=2 -DCONFIG_MPMC_WRITERS=1

%: $(TESTNAME).cc $(TESTNAME).h
	$(CXX) -o $@ $< $(CPPFLAGS) $(LDFLAGS)

clean:
	rm -f $(TESTS) *.o
