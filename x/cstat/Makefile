SHELL := /bin/bash
TARGETS := tabstat size

.PHONY: all
all: $(TARGETS)

%: %.c
	clang-format --style WebKit -i $^
	CFLAGS=-D_POSIX_C_SOURCE cc -o $@ $^

valgrind: all
	valgrind --leak-check=yes ./tabstat

.PHONY: clean
clean:
	rm -f $(TARGETS)
