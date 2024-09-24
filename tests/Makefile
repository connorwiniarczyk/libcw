includes += -I ../include
includes += -I ../src
includes += -I Unity/src
includes += -I $(HOME)/.local/include

cc_flags += $(includes)
cc_flags += -Wextra -Wall -Werror
cc_flags += -g
cc_flags += -DLINUX
cc_flags += -L$(HOME)/.local/lib
cc_flags += -lcanopen -lc

srcs += $(wildcard ../src/*.c)
objs += $(srcs:../src/%.c=build/%.o)
objs += build/unity.o

tests += run/cwfuture.test
tests += run/leak.test

all: build run

run: build/runner $(tests)
	@$< $(tests)
.PHONY: run

build/%.o: ../src/%.c
	@printf "CC\t$<\n"
	@$(CC) $(cc_flags) -o $@ -c $<

build/unity.o: Unity/src/unity.c
	@printf "CC\t$<\n"
	@$(CC) $(cc_flags) -o $@ -c $<

run/%.test: src/test_%.c $(objs)
	@mkdir -p run
	@printf "CC\t$<\n"
	@$(CC) $(objs) $(cc_flags) $< -o $@

build/runner: main.c
	@printf "CC\t$<\n"
	@$(CC) $(cc_flags) $< -o $@

debug:
	gdb $(TESTS)

clean:
	@echo "cleaning"
	-@rm -rf build
	-@rm -rf run

build:
	-@mkdir -p build
