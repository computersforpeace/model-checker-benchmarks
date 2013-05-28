include ../benchmarks.mk

TESTNAME = linuxrwlocks

all: $(TESTNAME)

$(TESTNAME): $(TESTNAME).c
	$(CC) -o $@ $< $(CFLAGS) $(LDFLAGS)

clean:
	rm -f $(TESTNAME) *.o
