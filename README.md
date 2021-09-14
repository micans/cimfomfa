# cimfomfa
Another blooming C utility library

This library supports both [MCL](http://github.com/micans/mcl), a cluster algorithm for graphs, and
[zoem](http://github.com/micans/zoem),
a macro/DSL language. It supplies abstractions for memory management, I/O,
associative arrays, strings, heaps, and a few other things.
The string library has had heavy testing as part of zoem. Both understandably
and regrettably I chose long ago to make it C-string-compatible, hence nul
bytes may not be part of a string. At some point I hope to rectify this, perhaps unrealistically.

Some testing programs are part of the library, found in `src/trumpet`.

More documentation to follow, always.


