include ../benchmarks.mk

TESTNAME = main

HEADERS = deque.h
OBJECTS = main.o deque.o

all: $(TESTNAME)

$(TESTNAME): $(HEADERS) $(OBJECTS)
	$(CC) -o $@ $(OBJECTS) $(CFLAGS) $(LDFLAGS)

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

clean:
	rm -f $(TESTNAME) *.o
