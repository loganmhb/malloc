# A Memory Allocator

This is a very basic toy memory allocator, inspired by the one in the
K&R book.

You use `new_malloc` and `new_free` just like `malloc` and `free`;
`new_malloc` returns `NULL` if no memory could be allocated. When
`new_free` is called, it traverses the free list and compacts adjacent
chunks in an attempt to preserve space.

There are two very basic tests. `printing_test.c` performs several
allocations and de-allocations and displays the contents of the free
list. `allocation_test` exercises the allocator by allocating and
freeing a billion chunks of memory.