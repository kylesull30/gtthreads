/**********************************************************************
gtthread_sched.c.  
This file contains the implementation of the scheduling subset of the
gtthreads library. 
 **********************************************************************/
#include "gtthread.h"
#include <stdio.h>
#include <stdlib.h>

/**********************************************************************
Global variables used by all functions
 **********************************************************************/
static int id = 0;
static int p;
static steque_t queue; // Data strcuture enforces round-robbin scheduling. 
static gtthread_t *running_thread;// A pointer to the thread that is running.
static gtthread_t *main_thread_pointer;//pointer to the main thread
static gtthread_t *next_thread; // The next thread that will run

/**********************************************************************
Both arrays are hard-coded for a maximum of 10 elements. This means that 
we are limited to 10 threads (including the main thread) until a more flexible 
data structure is implemented.
 **********************************************************************/

static int finished_array [10] = {0,0,0,0,0,0,0,0,0,0};// Contains int that indicates if the corresponding thread ID is finished (1) or not finished (0)
static gtthread_t* thread_pointers [10] = {0,0,0,0,0,0,0,0,0,0};//Same pointers that are in the queue are stored here. Constant time access to get them when needed.

/**********************************************************************
thread_runner allows us to capture the return value of a function when it finishes executing.
 **********************************************************************/
void thread_runner(void) {

	gtthread_t *calling_thread = next_thread;
	calling_thread->retval = calling_thread->func(calling_thread->arg);

	// Mark the thread that ran as finished
	finished_array[calling_thread->id] = 1;

		
	if(calling_thread->retval != NULL){
		gtthread_exit(calling_thread->retval);
 	} else {
    	gtthread_exit(NULL);
	}

	running_thread = main_thread_pointer; //running thread is now the main thread

}
/**********************************************************************
check_thread queries the finished array and returns an int corresponding 
to thread's status.
 **********************************************************************/
int check_thread(int id){
	return finished_array[id];
}

/**********************************************************************
gtthread_init initilizes the memory associated with the main thread and 
the scheduling queue.
 **********************************************************************/
void gtthread_init(long period){

	p = period; // Not used in non-preemptive version
	main_thread_pointer = (gtthread_t*) malloc(sizeof(gtthread_t));//Dynamically allocate memory for main thread's pointer
	main_thread_pointer->id = 0; // Set thread's ID as 0 because it is main
	main_thread_pointer->state = 0;
	steque_init(&queue);

	main_thread_pointer->context.uc_link = NULL; // When the main thread finishes, the process terminates
	
	thread_pointers [0] = main_thread_pointer; //Insert pointer into array indexed by thread ID

	running_thread = main_thread_pointer; // Main thread is running

 	steque_enqueue(&queue, main_thread_pointer);//Add the scheduling queue
}

/**********************************************************************
gtthread_create mirrors pthread_create() function
 **********************************************************************/
int gtthread_create(gtthread_t *thread, void *(*start_routine)(void *),
		    void *arg){

  gtthread_t *calling_thread;
	next_thread = (gtthread_t*) malloc(sizeof(gtthread_t));
	next_thread = thread;

	//Get context on the new thread.
	if (getcontext(&next_thread->context)  == -1){
  		perror("getcontext");
    		exit(EXIT_FAILURE);
	}
 
	
	// allocate stack
	next_thread->context.uc_stack.ss_sp = (char*) malloc(SIGSTKSZ);
	next_thread->context.uc_stack.ss_size = SIGSTKSZ;
	

	// Set successor
	next_thread->context.uc_link = &main_thread_pointer->context;
	
	//set starting routine
	next_thread->func = start_routine;
	next_thread->arg = arg;

	//makecontext(&next_thread->context, (void *)start_routine, 1, arg);
	makecontext(&next_thread->context, &thread_runner, 1, arg, NULL);
	
 	next_thread->id = ++id;
	next_thread->state = 0;
	
  // Swap context between the running thread and the new thread to start execution on the new thread.
	calling_thread = running_thread;
	running_thread = next_thread;

	thread_pointers[next_thread->id] =  next_thread;
	steque_enqueue(&queue, next_thread);

	if (swapcontext(&calling_thread->context, &next_thread->context) == -1){
    		perror("swapcontext");
   	 	exit(EXIT_FAILURE);
		return -1;
  	}
	running_thread = calling_thread; // After context switch the new thread is running

	return 0;
}

/**********************************************************************
gtthread_join() mirrors the pthread_join() function
 **********************************************************************/
int gtthread_join(gtthread_t thread, void **status){

	gtthread_t *thread_holder;
	int isDone = 0; // If 0 then not done, if 1 then done
	isDone = check_thread(thread.id);//Query finished array

	//Continue to execute if thread is not done
	while(isDone != 1){
		gtthread_yield();
		isDone = check_thread(thread.id);
	}

	//On return to successful call to gtthread_join pass the thread's return value to the *status parameter
	if(status){

		thread_holder = thread_pointers[thread.id];
		*status = thread_holder->retval;
			
	}

	return 0;
}

/**********************************************************************
  The gtthread_exit() function is analogous to pthread_exit().
 **********************************************************************/
void gtthread_exit(void* retval){

	running_thread->retval = retval;
	running_thread->state = 1; //The thread is finished


	if(running_thread->id == 0){ //If the running thread is the main thread
		gtthread_t *thread_holder;
		int count = 1;
							
		while(count!=id+1){

			thread_holder = thread_pointers[count];
			gtthread_join(*thread_holder, NULL);
			count++;
		}

	}
	else if(check_thread(running_thread->id) != 1) {
		finished_array[running_thread->id] = 1;
         
		if (swapcontext(&running_thread->context, &main_thread_pointer->context) == -1){
			perror("swapcontext");
				exit(EXIT_FAILURE);
			}  		
     }
				
}


/**********************************************************************
  The gtthread_yield() function is analogous to pthread_yield().
 **********************************************************************/
void gtthread_yield(void){
	// remove next thread from stack and set current thread to end of queue
	// set context to main, then implement as set to next in queue

	gtthread_t *calling_thread;

	if(steque_size(&queue)>0){ //check that queue is not empty
		
		next_thread = steque_front(&queue);//pull the next element from the queue

		steque_cycle(&queue);

		running_thread->state = 0;
 		calling_thread = running_thread;
		running_thread = next_thread;

		if(next_thread->state != 1){ //If gtthread_exit wasn't called then swapcontext and run this thread
			if (swapcontext(&calling_thread->context, &next_thread->context) == -1){
				perror("swapcontext");
				exit(EXIT_FAILURE);
	 		}
		}

		running_thread = calling_thread;

	}
	
}

/*
  The gtthread_equal() function is analogous to pthread_equal.
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
  The gtthread_cancel() function is analogous to pthread_cancel.
 */
int  gtthread_cancel(gtthread_t thread){

  	gtthread_t *thread_holder = thread_pointers[thread.id];
		finished_array[thread_holder->id] = 1;

    return 0;
}


/*
  Returns calling thread.
 */
gtthread_t gtthread_self(void){
	return *(running_thread);
}
