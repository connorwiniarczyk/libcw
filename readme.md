# libcw

libcw is a library I wrote for myself to make programming in C more productive
and enjoyable. C is a fantastic language, probably one of the best, but it's
standard library is, to put it lightly, not very good, at least by today's
standards. Like many before me, I've rolled my own in order to save myself
some pain and to formalize some of the techniques I've adopted for working
in this language.

## Memory Management

Inspired by zig, functions in libcw never implicitly allocate memory. Instead,
if a function needs to allocate memory, it takes an arena (CwArena) as one
of its arguments. In my implementation, arenas are structs with two pointers,
start and end, that represent a window into a continuous buffer. Allocation
is done by returning the start pointer, then incrementing it by the number
of bytes requested, and freeing memory is done by resetting the whole arena
back to some previous state. If an allocation would cause the start pointer
to exceed the end pointer, the program will panic and exit. I found that in
my cases, this is the behavior I wanted the overwhelming majority of the time,
and so it is the default, although it would be trivial to implement a "safe"
interface that requires the caller to check for NULL on each call.
