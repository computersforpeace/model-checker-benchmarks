DIRS := barrier mcs-lock mpmc-queue spsc-queue spsc-bugfix linuxrwlocks

.PHONY: $(DIRS)

all: $(DIRS)

clean: $(DIRS:%=clean-%)

$(DIRS):
	$(MAKE) -C $@

clean-%:
	-$(MAKE) -C $* clean
