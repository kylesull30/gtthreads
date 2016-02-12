//#define NDEBUG 1

#include "minunit.h"
#include "../gtthread.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include<sys/time.h>


static gtthread_t t_main, t1, t2;
static gtthread_mutex_t m1, m2;


static void thr1(void *in) {
    
    debug("thread 1 trying to lock");
    mu_assert(gtthread_mutex_lock(&m1) == 0, "Mutex lock failed but should have succeded");
    debug("thread 1 lock succeded");
    int i, j;
    for(i = 0; i < 10; i++){
        for(j = 0; j < 1000000;j++);
        debug("thr1...");
    }
    debug("thr1 done. unlocking");
    gtthread_mutex_unlock(&m1);
    
    return NULL;
}

static void thr2(void *in) {
    
    debug("thread 2 trying to lock");
    mu_assert(gtthread_mutex_lock(&m1) == 0, "Mutex lock failed but should have succeded");
    debug("thread 2 lock succeded");
    int i, j;
    for(i = 0; i < 10; i++){
        for(j = 0; j < 1000000;j++);
        debug("thr2...");
    }
    debug("thr2 done. unlocking");
    gtthread_mutex_unlock(&m1);
    
    return NULL;
}



char *test_mutex_init() {
    
    gtthread_mutex_init(&m1);
    mu_assert(m1.state == UNLOCKED, "Mutex initial state is not UNLOCKED");
    mu_assert(steque_size(&m1.waiters) == 0, "Mutex initial waiters is not 0");
    
    
    return NULL;
}

char *test_mutex_lock() {
    gtthread_mutex_t uninit_mutex;
    mu_assert(gtthread_mutex_lock(&uninit_mutex) == EINVAL, "Allowed to lock unitialized mutex");
    
    mu_assert(gtthread_mutex_lock(&m1) == 0, "Mutex lock failed but should have succeded");
    mu_assert(gtthread_mutex_lock(&m1) == EDEADLK, "Mutex owner allowed to lock its mutex");
    
    
    gtthread_create( &t1, thr1, NULL);
    gtthread_create( &t2, thr2, NULL);
    debug("Main working with lock");
    int i, j;
    for(i = 0; i < 10; i++){
        for(j = 0; j < 1000000;j++);
        debug("main...");
    }
    debug("Main done. unlocking");
    gtthread_mutex_unlock(&m1);
    debug("Main unlocked");
    
    gtthread_join(t1, NULL);
    gtthread_join(t2, NULL);
    
    return NULL;
}

char *test_mutex_unlock() {
    gtthread_mutex_t uninit_mutex;
    mu_assert(gtthread_mutex_unlock(&uninit_mutex) == EINVAL, "Allowed to unlock unitialized mutex");
    
    mu_assert(gtthread_mutex_unlock(&m1) == EPERM, "Mutex allowed for non-owner");
    
    gtthread_mutex_lock(&m1);
    mu_assert(gtthread_mutex_unlock(&m1) == 0, "Mutex unlock failed but should have succeeded");
    
    
    return NULL;
}


char *test_mutex_destroy() {
    gtthread_mutex_t uninit_mutex;
    mu_assert(gtthread_mutex_destroy(&uninit_mutex) == EINVAL, "Allowed to destroy unitialized mutex");
    
    gtthread_mutex_lock(&m1);
    mu_assert(gtthread_mutex_destroy(&m1) == EBUSY, "Allow destruction of locked mutex");
    gtthread_mutex_unlock(&m1);
    
    gtthread_mutex_destroy(&m1);
    mu_assert(m1.state == DESTROYED, "Mutex destroy state is not DESTROYED");
    
    return NULL;
}



char *all_tests() {
    // Init
    mu_suite_start();
    gtthread_init(500);
    t_main = gtthread_self();
    
    
    // Run Tests
    mu_run_test(test_mutex_init);
    mu_run_test(test_mutex_lock);
    mu_run_test(test_mutex_unlock);
    mu_run_test(test_mutex_destroy);
    
    
    debug("test done");
    
    return NULL;
}

RUN_TESTS(all_tests);