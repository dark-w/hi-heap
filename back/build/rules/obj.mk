include $(path)/Makefile
include build/rules/common.mk

PHONY := $(foreach n, $(PHONY), $(path)/$(n))
PHONY += $(dir-y)

subdir-objs := $(foreach n, $(dir-y), $(path)/$(n)/$(builtin-obj))
builtin-obj := $(path)/built-in.o

obj-y := $(foreach n, $(obj-y), $(path)/$(n))

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<

all: $(builtin-obj)

$(builtin-obj): $(subdir-objs) $(obj-y)
	$(LD) -r $^ -o $@

$(subdir-objs): $(dir-y)

$(dir-y):
	make $(obj_build)$(path)/$@

clean:
	@for dir in $(dir-y); do \
		 make $(obj_build)$(path)/$$dir clean; \
	 done
	@rm -vf $(path)/*.o

.PHONY: $(PHONY)
