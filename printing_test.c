#include "new_malloc.c"

int main() {
  printf("Start of memory block: %p\n", memory);
  void *ptr = new_malloc(16);
  print_free_list();
  printf("first pointer, to 16 bytes: %p\n", ptr);
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
  new_free(ptr);
  print_free_list();
  new_free(ptr6);
  new_free(ptr5);
  new_free(ptr3);
  print_free_list();
}
