prefix   ?= $(HOME)/.local
platform ?= linux
canopen  ?= false

MAKEFLAGS += --no-print-directory

cc_flags += -I include
cc_flags += -I include/cwutils
cc_flags += -g
cc_flags += -O2
cc_flags += -Wall -Wextra -Werror

srcs += src/cwarena.c
srcs += src/cwcmd.c
srcs += src/cwvector.c
srcs += src/cwqueue.c
srcs += src/cwfmt.c
srcs += src/cwarray.c
srcs += src/cwstring.c
srcs += src/cwbox.c
srcs += src/cwfuture.c
srcs += src/cwsleep.c
srcs += src/cwtimer.c

ifdef platform
srcs += src/platform_$(platform).c
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
	@cp include/cwgeometry.h $(prefix)/include/cwgeometry.h
	@cp include/cwplatform.h $(prefix)/include/cwplatform.h
	# @rsync -a include/cwutils/ $(prefix)/include/cwutils/
	# @mkdir -p $(prefix)/include/cwutils

