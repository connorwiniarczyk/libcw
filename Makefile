prefix = $(HOME)/.local
MAKEFLAGS += --no-print-directory

target ?= LINUX

cc_flags += -I include
cc_flags += -I include/cwutils
cc_flags += -g
cc_flags += -O2
cc_flags += -Wall -Wextra -Werror
cc_flags += -D$(target)

srcs += src/cwarray.c
srcs += src/cwstring.c
srcs += src/cwlog.c
srcs += src/cwlayout.c
srcs += src/cwfuture.c
srcs += src/cwsleep.c
srcs += src/cwtimer.c


canopen = true
ifdef canopen
srcs += src/cwcanopen.c
cc_flags += -I $(HOME)/.local/include
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
	@rm $(prefix)/lib/libcw.a
	@cp $< $(prefix)/lib

	@mkdir -p $(prefix)/include
	@mkdir -p $(prefix)/include/cwutils
	@rsync -a include/cwutils/ $(prefix)/include/cwutils/

