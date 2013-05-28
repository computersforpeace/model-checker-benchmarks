include ../benchmarks.mk

TESTNAME = dekker-fences

all: $(TESTNAME)

$(TESTNAME): $(TESTNAME).cc
	$(CXX) -o $@ $< $(CXXFLAGS) $(LDFLAGS)

clean:
	rm -f $(TESTNAME) *.o
