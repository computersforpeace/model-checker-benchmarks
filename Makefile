DIRS := barrier

.PHONY: subdirs $(DIRS)

all: $(DIRS)

$(DIRS):
	$(MAKE) -C $@
