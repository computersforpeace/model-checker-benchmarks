include ../benchmarks.mk

TESTNAME = main

HEADERS = main.h my_queue.h
OBJECTS = main.o my_queue.o args.o

all: $(TESTNAME)

$(TESTNAME): $(HEADERS) $(OBJECTS)
	$(CC) -o $@ $^ $(CPPFLAGS) $(LDFLAGS)

%.o: %.c
	$(CC) -c -o $@ $< $(CPPFLAGS)

clean:
	rm -f $(TESTNAME) *.o
