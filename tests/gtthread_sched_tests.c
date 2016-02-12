//#define NDEBUG 1

#include "minunit.h"
#include "../gtthread.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include<sys/time.h>
/* Tests
    - gtthread_create()
    - gtthread_equal()
*/

static gtthread_t t_main;
static gtthread_t t1;
static gtthread_t t2;

static void thr1(void *in) {
    sleep(1); // so that main has called join to this thread
    mu_assert(gtthread_join(t_main, NULL) == EDEADLK , "Two threads allowed to join each other");
    
    int retval = 23;
    gtthread_exit(&retval);
    
    return NULL;
}

static void thr2(void *in) {
    // To be cancelled immediately
    sleep(2);
    debug("Thread 2 ran");
    
    return NULL;
}

static void thr3(void *in) {
    sleep(6);
    
    return NULL;
}



char *test_gtthread_create() {
    gtthread_init(5000);
    
    t_main = gtthread_self();
    
    // test gtthread_create()
    gtthread_create( &t1, thr1, NULL);
    mu_assert(t1 != NULL, "Thread create returned NULL");
    
    
//    t1 = t;
//    printf("t is %p, t1 is %p\n", t, t1);
//    fflush(stdout);
//    gtthread_create( &t2, thr2, NULL);
//    mu_assert(t1 != NULL, "Thread create returned NULL");
//    
//    gtthread_create( &t3, thr3, NULL);
//    mu_assert(t1 != NULL, "Thread create returned NULL");
    
    
    return NULL;
}

char *test_gtthread_equal() {
    mu_assert(gtthread_equal(t1, gtthread_self()) == 0, "Different threads returned equal");
    mu_assert(gtthread_equal(gtthread_self(),gtthread_self()), "Same threads returned as different");
    
    return NULL;
}

char *test_gtthread_join() {
    mu_assert(gtthread_join(gtthread_self(), NULL) == EDEADLK , "Thread allowed to join itself");
    
    int* status;
    gtthread_join(t1, &status);
    mu_assert(*status == 23, "incorrect statusreturned from pthread_exit()");
    
    
    mu_assert(gtthread_join(t1, NULL) == EINVAL, "Thread allowed to join thread with waiter");
    
    
    gtthread_t bogus = malloc(sizeof(gtthread));
    mu_assert(gtthread_join(bogus, NULL) == ESRCH , "allowed to join nonexistent thread");
    free(bogus);
    
    
    gtthread_create( &t2, thr2, NULL);
    gtthread_cancel(t2);
    gtthread_join(t2, &status);
    debug("status %d", *status);
    mu_assert(*status == CANCELED, "Canceled thread did not return CANCELED");
    
    
    return NULL;
}

char *all_tests() {
    mu_suite_start();
    
    
    mu_run_test(test_gtthread_create);
    mu_run_test(test_gtthread_equal);
    mu_run_test(test_gtthread_join);
    debug("test done");
    
    return NULL;
}

RUN_TESTS(all_tests);