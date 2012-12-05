include ../benchmarks.mk

TESTNAME = dekker-fences

all: $(TESTNAME)

$(TESTNAME): $(TESTNAME).cc
	$(CXX) -o $@ $< $(CPPFLAGS) $(LDFLAGS)

clean:
	rm -f $(TESTNAME) *.o
