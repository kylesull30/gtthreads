#include <stdio.h>
#include <stdlib.h>
#include "gtthread.h"
void *worker(void *arg)
{
    int i;
 
    for (i = 0; i < 2; i++) {
        printf("thr1\n");
  			fflush(stdout);
        gtthread_yield();
    }
    return 23;
}

void *worker2(void *arg)
{
    int i;
 
    for (i = 0; i < 2; i++) {
        printf("thr2\n");
  			fflush(stdout);
        gtthread_yield();
    }
    return NULL;
}

void *worker3(void *arg)
{
    int i;
 
    for (i = 0; i < 2; i++) {
        printf("thr3\n");
  			fflush(stdout);
        gtthread_yield();
    }
    return NULL;
}
 
int main()
{
    int i;
    gtthread_t worker_tid, worker2_tid, worker3_tid;
	  gtthread_init(0);
    gtthread_create(&worker_tid, worker, NULL);
    gtthread_create(&worker2_tid, worker2, NULL);
    gtthread_create(&worker3_tid, worker3, NULL);
    for (i = 0; i < 2; i++) {
        printf("main\n");
  			fflush(stdout);
        gtthread_yield();
    }
 
    gtthread_join(worker_tid, NULL);
 		printf("Join 1 finished.\n");
  	fflush(stdout);
    gtthread_join(worker2_tid, NULL);
    gtthread_join(worker3_tid, NULL);
    puts("");
 		printf("main finished.\n");
  	fflush(stdout);
    return 0;


}
/* Tests creation.
   Should print "Hello World!" */

/*void *thr1(void *in) {

  printf("Hello World!\n");
  fflush(stdout);
 gtthread_yield();
	//exit(EXIT_SUCCESS);
  return 0;
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


  //fflush(stdout);
  gtthread_t t1, t2;
	int *status;
  gtthread_init(0);

  gtthread_create( &t1, thr1, NULL);
  printf("Thread %d (%p) was created.\n", t1.id, &t1);
  fflush(stdout);
	//gtthread_create( &t2, thr2, NULL);
	//gtthread_join(t1, status);
	//gtthread_yield();

	if (gtthread_join(t1, NULL) == 0) {
    printf("Finishing up!\n");
  }
 	//printf("Return value is %d.\n", *status);
  //fflush(stdout);
	printf("main: exiting\n");
  return EXIT_SUCCESS;
}*/
