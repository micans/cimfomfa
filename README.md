# cimfomfa
Another blooming C utility library

This library supports both MCL, a cluster algorithm for graphs, and zoem,
a macro/DSL language. It supplies abstractions for memory management, I/O,
associative arrays, strings, heaps, and a few other things.
The string library has had heavy testing as part of zoem. Both understandably
and regrettably I chose long ago to make it C-string-compatible, hence nul
bytes may not be part of a string.

Some testing programs are part of the library, found in `src/trumpet`.

More documentation to follow.


