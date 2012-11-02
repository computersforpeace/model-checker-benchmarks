include ../benchmarks.mk

TESTNAME = mpmc-queue
TESTS = mpmc-queue mpmc-1r2w mpmc-2r1w

all: $(TESTS)

%: %.cc $(TESTNAME).h
	$(CXX) -o $@ $< $(CPPFLAGS) $(LDFLAGS)

clean:
	rm -f $(TESTNAME) *.o
