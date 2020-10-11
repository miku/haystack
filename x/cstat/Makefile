SHELL := /bin/bash
TARGETS := tabstat

.PHONY: all
all: $(TARGETS)

%: %.c
	clang-format --style Google -i $^
	CFLAGS=-D_POSIX_C_SOURCE cc -o $@ $^

.PHONY: clean
clean:
	rm -f $(TARGETS)
