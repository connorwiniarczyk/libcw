prefix = $(HOME)/.local
MAKEFLAGS += --no-print-directory

cc_flags += -I include
cc_flags += -g
cc_flags += -O2
cc_flags += -Wall -Wextra -Werror

srcs += src/cwarray.c
srcs += src/cwstring.c
srcs += src/cwlog.c
srcs += src/cwlayout.c
srcs += src/cwfuture.c
objs += $(srcs:src/%.c=build/%.o)

$(info $(objs))

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
	@rsync -a include/ $(prefix)/include/cwutils/

