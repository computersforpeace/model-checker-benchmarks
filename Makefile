DIRS := barrier mcs-lock mpmc-queue spsc-queue

.PHONY: $(DIRS)

all: $(DIRS)

clean: $(DIRS:%=clean-%)

$(DIRS):
	$(MAKE) -C $@

clean-%:
	-$(MAKE) -C $* clean
