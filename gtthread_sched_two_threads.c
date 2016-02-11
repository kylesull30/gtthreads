/**********************************************************************
gtthread_sched.c.  

This file contains the implementation of the scheduling subset of the
gtthreads library.  A simple round-robin queue should be used.
 **********************************************************************/
/*
  Include as needed
*/

#include "gtthread.h"
#include <stdio.h>
#include <stdlib.h>
/* 
   Students should define global variables and helper functions as
   they see fit.
 */



static int id = 0;
static int p;
static steque_t queue;
static gtthread_t *running_thread;
static gtthread_t *main_thread;
//static ucontext_t uctx_thisRoutine;



/*
  The gtthread_init() function does not have a corresponding pthread equivalent.
  It must be called from the main thread before any other GTThreads
  functions are called. It allows the caller to specify the scheduling
  period (quantum in micro second), and may also perform any other
  necessary initialization.  If period is zero, then thread switching should
  occur only on calls to gtthread_yield().

  Recall that the initial thread of the program (i.e. the one running
  main() ) is a thread like any other. It should have a
  gtthread_t that clients can retrieve by calling gtthread_self()
  from the initial thread, and they should be able to specify it as an
  argument to other GTThreads functions. The only difference in the
  initial thread is how it behaves when it executes a return
  instruction. You can find details on this difference in the man page
  for pthread_create.
 */
void gtthread_init(long period){

	p = period;

	gtthread_t first_thread;
	first_thread.id = 0;

	steque_init(&queue);

	running_thread = &first_thread;	
	main_thread= running_thread;
  
	
}


/*
  The gtthread_create() function mirrors the pthread_create() function,
  only default attributes are always assumed.
 */
int gtthread_create(gtthread_t *thread, void *(*start_routine)(void *),
		    void *arg){
  printf("Entering create...\n");
  fflush(stdout);

	gtthread_t current_thread = *running_thread;

	gtthread_t next_thread;


	//ucontext_t current_thread_context = *current_thread.context;

  printf("Getting context...\n");
  fflush(stdout);
	//ucontext_t next_context = 
	if (getcontext(&next_thread.context)  == -1){
  	perror("getcontext");
    exit(EXIT_FAILURE);
	}
 

	// allocate stacks
	next_thread.context.uc_stack.ss_sp = (char*) malloc(SIGSTKSZ);
	next_thread.context.uc_stack.ss_size = SIGSTKSZ;


	// Set successor
	next_thread.context.uc_link = &current_thread.context;
	
	//set starting routine
	makecontext(&next_thread.context, (void *)start_routine, 1, NULL);

  next_thread.id = ++id;

  printf("Swapping Context...\n");
  fflush(stdout);
  
	if (swapcontext(&current_thread.context, &next_thread.context) == -1){
    perror("swapcontext");
		return -1;
    exit(EXIT_FAILURE);
  }
	
 	steque_enqueue(&queue, &current_thread);
	running_thread = &next_thread;

	return 0;
}

/*
  The gtthread_join() function is analogous to pthread_join.
  All gtthreads are joinable.
 */
int gtthread_join(gtthread_t thread, void **status){
// waits for thread to terminate
//while thread is running loop
 
	return 1;
}

/*
  The gtthread_exit() function is analogous to pthread_exit.
 */
void gtthread_exit(void* retval){

}


/*
  The gtthread_yield() function is analogous to pthread_yield, causing
  the calling thread to relinquish the cpu and place itself at the
  back of the schedule queue.
 */
void gtthread_yield(void){
	// remove next thread from stack and set current thread to end of queue
	// set context to main, then implement as set to next in queue

 	//setcontext(&main_thread.context);	
	//printf("Running thread is %d. There are %d elements in the queue.\n", running_thread.id, steque_size(&queue));
	 
	//gtthread_t running_thread = *(gtthread_t*)steque_pop(&queue);
	gtthread_t next_thread = *(gtthread_t*)steque_pop(&queue);
	gtthread_t current_thread = *running_thread;
	printf("Swapping back to main...\n");
  fflush(stdout);
	//setcontext(&next_thread.context);	
	if (swapcontext(&current_thread.context, &next_thread.context) == -1){
    perror("swapcontext");
    exit(EXIT_FAILURE);
  }

}

/*
  The gtthread_equal() function is analogous to pthread_equal,
  returning non-zero if the threads are the same and zero otherwise.
 */
int  gtthread_equal(gtthread_t t1, gtthread_t t2){

	return 0;
}

/*
  The gtthread_cancel() function is analogous to pthread_cancel,
  allowing one thread to terminate another asynchronously.
 */
int  gtthread_cancel(gtthread_t thread){
	return 0;
}

/*
  Returns calling thread.
 */
gtthread_t gtthread_self(void){
	//return main_thread;
}
