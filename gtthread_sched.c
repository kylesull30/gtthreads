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
static steque_t finished_queue;
static gtthread_t *running_thread;
static gtthread_t *main_thread_pointer;
static gtthread_t *next_thread;
static int finished_array [ 10] = {0,0,0,0,0,0,0,0,0,0};

//static void *arg;
//static gtthread_t main_thread;

//static ucontext_t uctx_thisRoutine;

// Thread runner

void thread_runner(void) {

		/*printf("Entering runner with thread %d running\n", running_thread->id);
  	fflush(stdout);*/

		void **retval = next_thread->func(next_thread->arg);
		/*printf("Thread %d finished.\n", running_thread->id);
  	fflush(stdout);*/

    if(retval != NULL){
			printf("Calling exit NOT null %d .\n", retval);
  		fflush(stdout);
    	gtthread_exit(retval);
 		} else {

			printf("Calling exit null.\n");
  		fflush(stdout);
    	gtthread_exit(NULL);
	}
 /* printf("Leaving runner with thread %d running\n", running_thread->id);
  fflush(stdout);*/
}

int check_thread(int id){
	/*steque_t queue_copy;
	steque_init(&queue_copy);
	queue_copy = finished_queue;
	int *id_holder = malloc(sizeof(int));
	int size = steque_size(&queue_copy);
	int counter = 0;
 printf("Finished queue size is %d \n", size);
  fflush(stdout);
	while(counter<size-1){
		printf("Finished queue size is %d and count is %d \n", size, counter);
  	fflush(stdout);
		id_holder = (int*) steque_pop(&queue_copy);
		printf("ID is %d \n", *id_holder);
  	fflush(stdout);
		steque_enqueue(&queue_copy, id_holder);
		counter++;
		if(*id_holder == id){
 			printf("Returning found thread finished. \n");
  		fflush(stdout);
			return 1;
		}
		//size = steque_size(&queue_copy);
	}
		return 0;*/
	return finished_array[id];
}
/*void* search_by_id(int id, int *valid){
	printf("Etnering search by ID.\n");
  fflush(stdout);
		
	int *intp;
	steque_t searchable_queue = queue;
	printf("Queue is %p (size %d) and searchable queue is %p (size %d).\n", &queue, steque_size(&queue), &searchable_queue, steque_size(&searchable_queue));
  fflush(stdout);
	gtthread_t *popped_thread;
	popped_thread = steque_pop(&searchable_queue);
	printf("Popped thread from steque of size %d (%p).\n", steque_size(&searchable_queue), popped_thread);
  fflush(stdout);
	/*if (popped_thread){
			printf("Removing finished thread.\n", steque_size(&searchable_queue), popped_thread);
  		fflush(stdout);
			popped_thread = steque_pop(&queue);
	}
	printf("Not null pointer.\n");
  fflush(stdout);
	while(popped_thread->id != id && steque_size(&searchable_queue)> 0){
		printf("Popping for search.\n");
  	fflush(stdout);
		
		popped_thread = steque_pop(&searchable_queue);
	*printf("Thread %d popped in state %d.\n", popped_thread->id, popped_thread->state);
  	fflush(stdout);

		//valid = intp;
	}
		if (popped_thread->id == id){
			*valid = 1;
		}
		else{
			*valid = 0;
		}
	return popped_thread;
}*/
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
	main_thread_pointer->state = 0;
	steque_init(&queue);
	steque_init(&finished_queue);
	main_thread_pointer->context.uc_link = NULL;
	running_thread = main_thread_pointer;
 	printf("Eqnueueing %d (%p) running.\n", running_thread->id, running_thread);
  fflush(stdout);	
 	steque_enqueue(&queue, main_thread_pointer);
}


/*
  The gtthread_create() function mirrors the pthread_create() function,
  only default attributes are always assumed.
 */
int gtthread_create(gtthread_t *thread, void *(*start_routine)(void *),
		    void *arg){
 	printf("Entering create with thread %d running.\n", running_thread->id);
  fflush(stdout);
  gtthread_t *calling_thread;
	next_thread = (gtthread_t*) malloc(sizeof(gtthread_t));
	next_thread = thread;

	//Get context on the new thread.
	if (getcontext(&next_thread->context)  == -1){
  		perror("getcontext");
    		exit(EXIT_FAILURE);
	}
 
	
	// allocate stacks
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
	printf("Enqueueing %d (%p) running.\n", next_thread->id, next_thread);
  fflush(stdout);	
	steque_enqueue(&queue, next_thread);
	if (swapcontext(&calling_thread->context, &next_thread->context) == -1){
    		perror("swapcontext");
   	 	exit(EXIT_FAILURE);
		return -1;
  	}


	running_thread = calling_thread;

	/*	printf("Leaving create with thread %d running\n", running_thread->id);
  	fflush(stdout);*/
    //running_thread = main_thread_pointer;

	return 0;
}

/*
  The gtthread_join() function is analogous to pthread_join.
  All gtthreads are joinable.
 */
int gtthread_join(gtthread_t thread, void **status){

	//gtthread_t *calling_thread, *translate_thread;
	int isDone = 0;

		isDone = check_thread(thread.id);

		while(isDone != 1){
			gtthread_yield();
			isDone = check_thread(thread.id);
	}
 
	status = thread.retval;
		printf("Join returning status %d for thread %d\n",thread.retval, running_thread->id);
  	fflush(stdout);
  /*printf("Leaving join with thread %d running\n", running_thread->id);
  fflush(stdout);*/
	return 0;
}

/*
  The gtthread_exit() function is analogous to pthread_exit.
 */
void gtthread_exit(void* retval){
		printf("Entering exit with thread %d running\n", running_thread->id);
  	fflush(stdout);
		running_thread->retval = retval;
		running_thread->state = 1;
		//gtthread_t *thread_holder = steque_pop(&queue);
		//steque_enqueue(&finished_queue, &running_thread->id);
		// remove from queue
		finished_array[running_thread->id] = 1;
		printf("Added %d to finished\n", running_thread->id);
  	fflush(stdout);

		running_thread = main_thread_pointer;
		/*printf("Leavings exit with thread %d running\n", running_thread->id);
  	fflush(stdout);*/
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

  	printf("Entering yield with thread %d running\n", running_thread->id);
  	fflush(stdout);
		gtthread_t *calling_thread;

	 if(steque_size(&queue)>0){
		
		next_thread = steque_front(&queue);
 		printf("Read %d (%p) from queue.\n", next_thread->id, next_thread);
  	fflush(stdout);	
		steque_cycle(&queue);
 		printf("Cycled %d (%p) to back.\n", next_thread->id, next_thread);
  	fflush(stdout);
		running_thread->state = 0;
    calling_thread = running_thread;
		running_thread = next_thread;
 		printf("Attempting to swap %d (%p).\n", next_thread->id, next_thread);
  	fflush(stdout);
		if(next_thread->state != 1){
			if (swapcontext(&calling_thread->context, &next_thread->context) == -1){
   	 		perror("swapcontext");
    		exit(EXIT_FAILURE);
  		}
		}


		/*if (size_before == size_after){
					
		}*/
		running_thread = calling_thread;

	/*	printf("Leaving yield with thread %d running\n", running_thread->id);
  	fflush(stdout);*/

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
	//thread.state = 1;
	// do cancel stuff
  gtthread_exit(thread.retval);
	return 0;
}


/*
  Returns calling thread.
 */
gtthread_t gtthread_self(void){
	return *(main_thread_pointer);
}
