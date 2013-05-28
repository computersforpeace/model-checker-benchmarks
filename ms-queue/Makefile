include ../benchmarks.mk

TESTNAME = main

HEADERS = my_queue.h
OBJECTS = main.o my_queue.o

all: $(TESTNAME)

$(TESTNAME): $(HEADERS) $(OBJECTS)
	$(CC) -o $@ $(OBJECTS) $(CFLAGS) $(LDFLAGS)

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

clean:
	rm -f $(TESTNAME) *.o
