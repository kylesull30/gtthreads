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
static gtthread_t *main_thread_pointer;
//static gtthread_t main_thread;

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
	main_thread_pointer = (gtthread_t*) malloc(sizeof(gtthread_t));
	main_thread_pointer->id = 0;
	steque_init(&queue);
	main_thread_pointer->context.uc_link = NULL;
	running_thread = main_thread_pointer;	
	//main_thread_pointer= &main_thread;
 	steque_enqueue(&queue, main_thread_pointer);
}


/*
  The gtthread_create() function mirrors the pthread_create() function,
  only default attributes are always assumed.
 */
int gtthread_create(gtthread_t *thread, void *(*start_routine)(void *),
		    void *arg){
 	printf("Etnering create with thread %d running.\n", running_thread->id);
  	fflush(stdout);
    	gtthread_t *next_thread, *swappable_running_thread;
	next_thread = (gtthread_t*) malloc(sizeof(gtthread_t));
	//Get context on the new thread.
	if (getcontext(&next_thread->context)  == -1){
  		perror("getcontext");
    		exit(EXIT_FAILURE);
	}
 
	printf("Thread %d running after get_context.\n", running_thread->id);
  	fflush(stdout);
	// allocate stacks
	next_thread->context.uc_stack.ss_sp = (char*) malloc(SIGSTKSZ);
	next_thread->context.uc_stack.ss_size = SIGSTKSZ;
	printf("Thread %d running after allocating memory.\n", running_thread->id);
  	fflush(stdout);

	// Set successor
	next_thread->context.uc_link = &main_thread_pointer->context;
	printf("Thread %d running after setting context link.\n", running_thread->id);
  	fflush(stdout);
	//set starting routine
	makecontext(&next_thread->context, (void *)start_routine, 1, NULL);
	printf("Thread %d running after makecontext.\n", running_thread->id);
  	fflush(stdout);
 	next_thread->id = ++id;
	printf("Thread %d running after changing the id.\n", running_thread->id);
  	fflush(stdout);
	printf("Swapping from thread %d to %d.\n", running_thread->id, next_thread->id);
  	fflush(stdout);
  	// Swap context between the running thread and the new thread to start execution on the new thread.
	swappable_running_thread = running_thread;
	running_thread = next_thread;
	if (swapcontext(&swappable_running_thread->context, &next_thread->context) == -1){
    		perror("swapcontext");
   	 	exit(EXIT_FAILURE);
		return -1;
  	}
	
	//running_thread = next_thread;

	printf("Context swapped in create and running thread is now %d.\n", running_thread->id);
  	fflush(stdout);
	return 0;
}

/*
  The gtthread_join() function is analogous to pthread_join.
  All gtthreads are joinable.
 */
int gtthread_join(gtthread_t thread, void **status){
// waits for thread to terminate
//while thread is running loop
 
	return ;
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
	//gtthread_t current_thread;
  	printf("Entering yield....\n");
  	fflush(stdout);
	 if(steque_size(&queue)>0){
		printf("Popping next thread from queue\n");
  		fflush(stdout);
		gtthread_t *next_thread = steque_pop(&queue);
		printf("Enqueueing current thread to queue\n");
  		fflush(stdout);
		steque_enqueue(&queue, running_thread);

		if (swapcontext(&running_thread->context, &next_thread->context) == -1){
			printf("EError in context switch.\n");
  			fflush(stdout);
   	 		perror("swapcontext");
    			exit(EXIT_FAILURE);
  		}
		running_thread = next_thread;
		printf("Context swapped in yield.\n");
  		fflush(stdout);

	}
	else{
  	printf("Nothing in queue....\n");
  	fflush(stdout);
		if (setcontext(&main_thread_pointer->context) == -1){
            		 perror("swapcontext");
            		exit(EXIT_FAILURE);
        	}

	}

}

/*
  The gtthread_equal() function is analogous to pthread_equal,
  returning non-zero if the threads are the same and zero otherwise.
 */
int  gtthread_equal(gtthread_t t1, gtthread_t t2){
	if(t1.id == t2.id){
		return 1;
	}
 else{
			return 0;
	}

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
	return *(main_thread_pointer);
}
