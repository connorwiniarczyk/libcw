# libcw

The goal of libcw is to replace the C standard library with something more
modern and better suited to the way I like to program C. It consists of a
number of low level tools for stuff like memory management, string manipulation
and formatting, dynamic arrays, etc. as well as host specific tools for
stuff like file and socket manipulation and running commands. C is pretty far
from being a perfect language, but despite decades of trying, nobody has really
been able to replace it, and so many of the problems with it are actually
just problems with the standard library. By replacing it, I've found working
with C to be significantly more productive and enjoyable, it has quickly become
my go to tool for just about every problem.

```c
#include <cwcore.h>
#include <cwhost.h>

int main() {
	// create an arena 2kB wide with a call to malloc
	CwArena a = cwarena_create(malloc, 1 << 11);

	// init the global logger with a buffer 256 bytes wide
	cwlog_init(&a, 1 << 8);

	// read the contents of input.csv into the arena
	cwlog("reading input.csv");
	CwStr file = cwhost_read_file(&a, "input.csv");
	if (file.size < 1) cwpanic("could not open file");

	// strings in libcw (CwStr) store size as a struct field
	// instead of using null terminators. This makes them considerably
	// easier to work with

	int i = 0;
	int j = 0;

	while (file.size) {
    	CwStr line = cwstr_split(&file, '\n');
    	while (line.size) {
    		CwStr item = cwstr_split(&line, ',');
    		cwlog("line %d, column %d: %w", i, j, item);
    		j += 1;
    	}
		i += 1;
		cwlog("\n");
	}

	cwlog("done");
}
```

## Memory Management

Inspired by zig, functions in libcw never implicitly allocate memory. Instead,
if a function needs to allocate memory, it takes an arena as one
of its arguments. Arenas represent a window into a single continuous region of
memory. Memory is allocated by shrinking the size of this window, and it is freed
by resetting the window back to some previous state. This introduces a lot of
restrictions not found in general purpose memory allocators, but I find the simplicity
gained in both implementation and use to be well worth the tradeoff.

If an allocation would cause an arena to shrink to less than 0 size, its default
behavior is to immediately exit the program by invoking cwpanic(). Since panicking
is the behavior I usually want when running out of memory, it made the most sense
to make that the default and to handle other cases specially. This has saved me
from what would otherwise be dozens of `assert(ptr != NULL);` clauses in every piece
of software I write.
