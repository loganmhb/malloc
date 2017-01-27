/* A basic memory-allocator */
#include "new_malloc.h"
#include <stdlib.h>
#include <stdio.h>

#define MEMSIZE 1048576

/*
 * Avoid any system memory considerations by using
 * statically-allocated memory. The free list will start as one block
 * pointing to this statically-allocated chunk.
 */
static char memory[MEMSIZE];

/* Header for a memory block.
 * 
 * `next` points to the header of the next block in the free list;
 * `size` indicates how much memory is allocated for this block. */
typedef struct header {
  struct header *next;
  unsigned size;
} header;

/* Pointer to the beginning of the free list. */
static header *freep = NULL;


/* Size is in bytes. As an implementation detail it gets rounded up
 * to increments of 8 bytes, to simplify the bookkeeping. */
void *new_malloc(unsigned required_size) {
  /* Make sure required_size is a multiple of sizeof(header), so we
   * can do simpler pointer arithmetic. */
  while ((required_size % sizeof(header)) != 0) required_size++;
  printf("Required size set at %d\n", required_size);

  /* Special case to initialize the free list. */
  if (freep == NULL) {
    freep = (header *)memory;
    freep->size = MEMSIZE - sizeof(header);
    freep->next = NULL;
  }

  header *p = freep;
  header *prev = NULL;
  /* Find the first big-enough block in the free list. */
  while(p->size < required_size) {
    prev = p;
    p = p->next;
    if (p == NULL) {
      /* No big-enough blocks. Oh well. */
      return NULL;
    }
  }

  /* Now, p should be a big-enough block on the free list. But it
   * might be way too big! Time to cut it down to size and keep what's
   * left for later. */
  void *allocated_ptr = p + 1;  // (what we'll return to the caller)
  header *new_free_block = p + (required_size / sizeof(header)) + sizeof(header);

  /* Put the new free block in the free list. */
  new_free_block->size = p->size - (required_size + sizeof(header));
  new_free_block->next = p->next;
  p->size = required_size;

  if (prev == NULL) {
    freep = new_free_block; // it's the beginning of the list
  } else {
    prev->next = new_free_block;
  }
  
  return allocated_ptr;
}


void new_free(void *ptr) {
  header *h = (header*)ptr - 1;
  h->next = freep;
  freep = h;
}


void print_free_list() {
  header *ptr = freep;
  puts("\n*** FREE LIST ***");
  do {
    printf("Header: %p, size %d, next %p\n", ptr, ptr->size, ptr->next);
  } while ((ptr = ptr->next) != NULL);
  puts("***\n");
}


int main() {
  printf("Start of memory block: %p\n", memory);
  void *ptr = new_malloc(16);
  print_free_list();
  printf("first pointer, to 16 bytes: %p\n", ptr);
  void *ptr2 = new_malloc(18);
  printf("second pointer, to 18 bytes: %p\n", ptr2);
  void *ptr3 = new_malloc(8);
  printf("third pointer, to 8 bytes: %p\n", ptr3);
  printf("free list location: %p\n", freep);
  new_free(ptr);
  new_free(ptr2);

  void *ptr5 = new_malloc(43);
  void *ptr6 = new_malloc(5);
  void *ptr7 = new_malloc(23);
  new_free(ptr6);
  new_free(ptr5);
  print_free_list();
}
