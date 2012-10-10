DIRS := barrier mcs-lock

.PHONY: subdirs $(DIRS)

all: $(DIRS)

$(DIRS):
	$(MAKE) -C $@
