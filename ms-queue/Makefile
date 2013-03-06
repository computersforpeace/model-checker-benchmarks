include ../benchmarks.mk

TESTNAME = main

HEADERS = my_queue.h
OBJECTS = main.o my_queue.o

all: $(TESTNAME)

$(TESTNAME): $(HEADERS) $(OBJECTS)
	$(CC) -o $@ $(OBJECTS) $(CPPFLAGS) $(LDFLAGS)

%.o: %.c
	$(CC) -c -o $@ $< $(CPPFLAGS)

clean:
	rm -f $(TESTNAME) *.o
