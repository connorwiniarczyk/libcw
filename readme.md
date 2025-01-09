# libcw

libcw is a personal set of pure C utility libraries to help make up for
the languages lack of a modern standard library. It implements a simple
arena allocator and builds on top of that a sort of psuedo standard library
complete with dynamic arrays, strings, something resembling async/await,
and anything else I can think to throw in here.


## Arena Allocators

libcw's allocator is called CwArena. Arenas allocate blocks of memory linearly
into a single continuous buffer. Instead of calling free on individual pointers,
arenas free memory by resetting themselves back to a previous state. This means
that if any memory is freed, all the memory allocated after it is freed as well.
This is slightly more limiting than the traditional way, but has some distinct
advantages.

```c
#include <cwutils/cwarena.h>

int main() {
	CwArena a = cwarena_new(1 << 10);

}
```

