include ../benchmarks.mk

TESTNAME = mcs-lock

all: $(TESTNAME)

$(TESTNAME): $(TESTNAME).cc $(TESTNAME).h
	$(CXX) -o $@ $< $(CXXFLAGS) $(LDFLAGS)

clean:
	rm -f $(TESTNAME) *.o
