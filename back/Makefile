CC = gcc
LD = ld

CFLAGS = -Wall -Werror -O2 -Iinclude
# LDFLAGD

MAKEFLAGS = --no-print-directory

builtin-obj = built-in.o

export CC LD CFLAGS MAKEFLAGS
export builtin-obj

dir-y = src lib
subdir-objs = $(foreach n, $(dir-y), $(n)/$(builtin-obj))

include build/rules/common.mk

all: the-back 

the-back: $(subdir-objs)
	$(CC) $(CFLAGS) $^ -o $@

$(subdir-objs): $(dir-y)

$(dir-y):
	make $(obj_build)$@

clean:
	@for dir in $(dir-y); do \
		 make $(obj_build)$$dir clean; \
	 done
	@rm -vf the-back

.PHONY: $(dir-y) the-back 
