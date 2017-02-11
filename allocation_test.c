#include "new_malloc.c"


int main() {
  /* Here we test the malloc implementation by allocating and then
     freeing *a lot* of small memory chunks, slightly shuffled. */
  void *prev = NULL, *current = NULL;
  for (int i = 0; i < 1000000000; i++) {
    prev = current;
    current = new_malloc(i%256);
    if (prev != NULL) {
      new_free(prev);
    }
  }
  print_free_list();
}
