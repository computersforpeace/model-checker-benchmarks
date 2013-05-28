include ../benchmarks.mk

TESTNAME = spsc-queue
RELACYNAME = spsc-relacy

all: $(TESTNAME)

$(TESTNAME): $(TESTNAME).cc queue.h eventcount.h
	$(CXX) -o $@ $< $(CXXFLAGS) $(LDFLAGS)

relacy: $(RELACYNAME)

$(RELACYNAME): spsc-relacy.cc queue-relacy.h eventcount-relacy.h
ifdef RELACYPATH
	$(CXX) -o $(RELACYNAME) spsc-relacy.cc -I$(RELACYPATH) -Wno-deprecated
else
	@echo "Please define RELACYPATH"
	@echo "  e.g., make RELACYPATH=/path-to-relacy"
	@exit 1
endif

clean:
	rm -f $(TESTNAME) $(RELACYNAME) *.o
