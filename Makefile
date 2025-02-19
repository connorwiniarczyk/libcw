prefix  ?= $(HOME)/.local
host    ?= linux
canopen ?= false

MAKEFLAGS += --no-print-directory

cc_flags += -I include
cc_flags += -g
cc_flags += -O2
cc_flags += -Wall -Wextra -Werror

srcs += src/cwarena.c
srcs += src/cwpool.c
srcs += src/cwarray.c
srcs += src/cwstring.c
srcs += src/cwfmt.c
srcs += src/cwgeometry.c
srcs += src/cwfuture.c

ifeq ($(host), linux)
srcs += src/host/linux.c
srcs += src/host/linux_log.c
srcs += src/host/linux_file.c
srcs += src/host/linux_build.c
endif

ifeq ($(canopen), true)
srcs += src/cwcanopen.c
cc_flags += -I $(prefix)/include
endif

objs += $(srcs:src/%.c=build/%.o)

build/libcw.a: clean $(objs)
	@mkdir -p build
	@printf "AR\t$@\n"
	@$(AR) -rc $@ $(objs)

build/%.o: src/%.c
	@mkdir -p build
	@mkdir -p build/host
	@printf "CC\t$<\n"
	@$(CC) $(cc_flags) -o $@ -c $<

clean:
	@printf "CLEAN\n"
	@rm -rf build
.PHONY: clean

install: build/libcw.a
	@printf "INSTALLING\n"
	@mkdir -p $(prefix)/lib
	@rm -f $(prefix)/lib/libcw.a
	@cp $< $(prefix)/lib

	@mkdir -p $(prefix)/include
	@cp include/cwcore.h $(prefix)/include/cwcore.h
	@cp include/cwhost.h $(prefix)/include/cwhost.h
	@cp include/cwgeometry.h $(prefix)/include/cwgeometry.h
