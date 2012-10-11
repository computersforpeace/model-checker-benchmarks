include ../benchmarks.mk

TESTNAME = spsc-queue

all: $(TESTNAME)

$(TESTNAME): $(TESTNAME).cc queue.h eventcount.h
	$(CXX) -o $@ $< $(CPPFLAGS) $(LDFLAGS)

clean:
	rm -f $(TESTNAME) *.o
