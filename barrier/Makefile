include ../benchmarks.mk

TESTNAME = barrier

all: $(TESTNAME)

$(TESTNAME): $(TESTNAME).cc $(TESTNAME).h
	$(CXX) -o $@ $< $(CXXFLAGS) $(LDFLAGS)

clean:
	rm -f $(TESTNAME) *.o
