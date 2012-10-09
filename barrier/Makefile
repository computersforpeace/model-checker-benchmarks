include ../benchmarks.mk

BIN = barrier

all: barrier

barrier: barrier.cc barrier.h
	$(CXX) -o $@ $< $(CPPFLAGS) $(LDFLAGS)

clean:
	rm -f $(BIN) *.o
