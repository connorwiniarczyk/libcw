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

### Example: Parsing a CSV file

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

    // strings in libcw store size directly instead of relying on null
    // terminators. This makes them considerably easier to work with

    int i = 0;
    while (file.size) {
        CwStr line = cwstr_split(&file, '\n');
        int sum = 0;
        while (line.size) {
            CwStr item = cwstr_split(&line, ',');
            sum += cwparse_int(item);
        }

        cwlog("line %2d: ", sum);
        i += 1;
    }
}
```

### Arenas

Functions in libcw never implicitly allocate memory. Instead,
if a function needs to allocate memory, it takes an arena as one
of its arguments. Arenas represent a window into a single continuous region of
memory. Memory is allocated by shrinking the size of this window, and it is freed
by resetting the window back to some previous state. This introduces a lot of
restrictions not found in general purpose memory allocators, but I find the simplicity
gained in both implementation and use to be well worth the tradeoff. Memory is
allocated with a call to the cwalloc() function or to the macro cwnew().

```c
// cwcore.h
void* cwalloc(CwArena* a, ptrdiff_t size, ptrdiff_t align, ptrdiff_t count);
#define cwnew(a, t) cwalloc(a, sizeof(t), alignof(t), 1)
```

If an allocation would cause an arena to shrink to less than 0 size, its default
behavior is to immediately exit the program by invoking cwpanic(). Since panicking
is the behavior I usually want when running out of memory, it made the most sense
to make that the default and to handle other cases specially. This has saved me
from what would otherwise be dozens of `assert(ptr != NULL);` clauses in every piece
of software I write.

### Dynamic Arrays

C's lack of support for templating makes true dynamic arrays very difficult to
write. Every implementation I've seen seems to be either too awkward or too liberal
with it's use of macros. Thankfully, in a lot of cases, dynamic arrays aren't
necessary. Instead, we can use arenas. Because memory in an arena is guaranteed
to be allocated in order, arrays can be constructed and added to simply by
allocating memory.

The following snippet generates a null terminated array of strings with the contents
["one", "two", "three"], simply by making repeated calls to cwnew(). It is important
to call `cwarena_align_to()` when generating the base pointer to make sure its
alignment matches that of the elements.

```c
CwArena a = cwarena_create(malloc, 10 * sizeof(char**));
char** array = cwarena_align_to(&a, alignof(char*));
*((char*)(cwnew(&a, char*))) = "one";
*((char*)(cwnew(&a, char*))) = "two";
*((char*)(cwnew(&a, char*))) = "three";
*((char*)(cwnew(&a, char*))) = NULL;
```

Because this syntax can be a little tricky, some macros
are provided to make this a little easier to work with.

```c
CwArena a = cwarena_create(malloc, 10 * sizeof(char**));
char** array = cwarray_begin(&a, char*);
cwarena_push(&a, char*, "one");
cwarena_push(&a, char*, "two");
cwarena_push(&a, char*, "three");
cwarena_push(&a, char*, NULL);
```
