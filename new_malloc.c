/* A basic memory-allocator */
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
static header *free_list = NULL;


/* Size is in bytes. As an implementation detail it gets rounded up
 * to increments of sizeof(header) bytes, to simplify the bookkeeping. */
void *new_malloc(unsigned required_size) {
  /* Make sure required_size is a multiple of sizeof(header), so we
   * can do simpler pointer arithmetic. */
  while ((required_size % sizeof(header)) != 0) required_size++;

  /* Special case to initialize the free list. */
  if (free_list == NULL) {
    free_list = (header *)memory;
    free_list->size = MEMSIZE - sizeof(header);
    free_list->next = NULL;
  }

  header *current = free_list;
  header *prev = NULL;
  /* Find the first big-enough block in the free list. */
  while(current->size < required_size) {
    prev = current;
    current = current->next;
    if (current == NULL) {
      /* No big-enough blocks. Oh well, nothing we can do. */
      return NULL;
    }
  }

  void *allocated_ptr = current + 1;  // (what we'll return to the caller)

  if (required_size < current->size) {
    /* Now, current should be a big-enough block on the free list. But
     * it might be way too big! Time to cut it down to size and keep
     * what's left for later. */
    header *new_free_block = current + (required_size / sizeof(header)) + sizeof(header);

    /* Put the new free block in the free list, in the correct (sorted) position. */
    new_free_block->size = current->size - (required_size + sizeof(header));
    new_free_block->next = current->next;
    current->size = required_size;

    if (prev == NULL) {
      free_list = new_free_block;
    } else {
      prev->next = new_free_block;
    }
  } else {
    /* We need to return the whole block, so we have to take it out of
       the free list. */
    if (prev == NULL) {
      free_list = current->next;
    } else {
      prev->next = current->next;
    }
  }
    
  return allocated_ptr;
}

/* As small chunks of memory are allocated and de-allocated,
   eventually the whole free list would consist of tiny chunks of
   memory and there wouldn't be any big chunks left over when we need
   them. To avoid that problem, we can detect when two adjacent small
   chunks are free at the same time and combine them. */
void compact_free_list() {
  header *current;
  current = free_list;
  while (current != NULL) {
    if ((current + 1 + (current->size / sizeof(header))) == current->next) {
      current->size = current->size + sizeof(header) + current->next->size;
      current->next = current->next->next;
    } else {
      current = current->next;
    }
  }
}


/* Freeing, I have arbitrarily decided, is when we will merge adjacent
   small chunks of memory together to minimize fragmentation. This may
   have undesirable performance characteristics. To make things more
   consistent, we insert blocks into the free list sorted by size, to
   avoid a potentially surprising costly sort. */
void new_free(void *ptr) {
  /* Extract the header from its hiding place just before the memory
     block ptr points to. */
  header *h = (header*)ptr - 1;

  header *prev = NULL;
  header *current = free_list;

  while(h > current && current != NULL) {
    /*    printf("%p is greater than than %p\n", h, current);
          printf("prev %p, current %p\n", prev, current);*/
    prev = current;
    current = current->next;
  }
  if(h == current) printf("Freeing already freed pointer!\n");

  /* At this point, one of three conditions holds:
     1) current is NULL (meaning h is the highest memory address in the free
     list and it just needs to be added to the end).
     2) prev is NULL, meaning h is the new first item in the list
     3) neither is NULL, in which case h needs to be inserted between them.
  */
  if (current == NULL) {
    prev->next = h;
  } else if (prev == NULL) {
    h->next = free_list;
    free_list = h;
  } else {
    h->next = current;
    prev->next = h;
  }
  compact_free_list();
}


void print_free_list() {
  header *ptr = free_list;
  puts("\n*** FREE LIST ***");
  int i = 0;
  do {
    printf("Header: %p, size %d, next %p\n", ptr, ptr->size, ptr->next);
  } while ((ptr = ptr->next) != NULL && i++ < 10);
  puts("***\n");
}


int main() {
  printf("Start of memory block: %p\n", memory);
  void *ptr = new_malloc(16);
  print_free_list();
  printf("first pointer, to 16 bytes: %p\n", ptr);
  new_free(ptr);
  print_free_list();
  void *ptr2 = new_malloc(16);
  printf("second pointer, to 18 bytes: %p\n", ptr2);
  void *ptr3 = new_malloc(8);
  printf("third pointer, to 8 bytes: %p\n", ptr3);
  printf("free list location: %p\n", free_list);
  new_free(ptr2);

  void *ptr4 = new_malloc(43);
  void *ptr5 = new_malloc(5);
  print_free_list();
  void *ptr6 = new_malloc(23);
  new_free(ptr4);
  new_free(ptr6);
  new_free(ptr5);
  new_free(ptr3);
  print_free_list();
}
