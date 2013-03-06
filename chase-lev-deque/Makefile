include ../benchmarks.mk

TESTNAME = main

HEADERS = 
OBJECTS = deque.o

all: $(TESTNAME)

$(TESTNAME): $(HEADERS) $(OBJECTS)
	$(CC) -o $@ $(OBJECTS) $(CPPFLAGS) $(LDFLAGS)

%.o: %.c
	$(CC) -c -o $@ $< $(CPPFLAGS)

clean:
	rm -f $(TESTNAME) *.o
