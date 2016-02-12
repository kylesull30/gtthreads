#include <stdio.h>
#include <stdlib.h>
#include "gtthread.h"

/* Tests creation.
   Should print "Hello World!" */

void *thr1(void *in) {

  printf("Hello World!\n");
  fflush(stdout);
 // gtthread_yield();

  return NULL;
}

void *thr2(void *in) {

  printf("Hello World 2!\n");
  fflush(stdout);
   //gtthread_yield();

  return NULL;
}
int main() {
  printf("Running main!\n");
  fflush(stdout);
  gtthread_t t1, t2;

  gtthread_init(0);

  gtthread_create( &t1, thr1, NULL);
gtthread_create( &t2, thr2, NULL);
gtthread_yield();

	printf("main: exiting\n");
  return EXIT_SUCCESS;
}
