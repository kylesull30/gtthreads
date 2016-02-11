#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <inttypes.h>
#include <pthread.h>

#include "philosopher.h"
pthread_mutex_t done_mutex = PTHREAD_MUTEX_INITIALIZER;//Mutex that protects the conditional variables and the states array
pthread_cond_t phil_waits[5];// Pthread condition variable to allow threads to communicate and control execution
int phil_states[5];// maintains state information for all five philosophers. States are: 0 = hungry, 1 = eating, 2 = thinking
long i;// used druing init and destroy functions to iterate through the two arrays
typedef enum  {left, right} direction_t; // used to calculate neighbors

/*
 * Initializes mutex resources.
 */

void chopsticks_init(){

 	for ( i = 0; i < 5; i++){
		phil_states[i] = 2;
		pthread_cond_init(&phil_waits[i], NULL);
	}
}

/*
 * Cleans up mutex resources.
 */
void chopsticks_destroy(){
	pthread_mutex_destroy(&done_mutex);
	for ( i = 0; i < 5; i++){
		pthread_cond_destroy(&phil_waits[i]);
	}
}
/*
* Converts a philosopher ID and direction to a neighboring philosopher ID
*/

int phil_to_neighbor(int phil_id, direction_t d){
  int movement;
  if(d == left){
     movement = phil_id+1;
     return (movement % 5);
  }
  else{
     movement = phil_id+4;
     return ((movement) % 5);
  }

}

/*
 * Checks to see if right and left neighbors are in state 1 (eating) and if philosopher of given ID is in state 0 (hungry)
 * If that is true then the philosopher of given ID changes to state 1 (eating) and signals to unblock one of the other threads
 * that is waiting.
 */ 
void check_neighbors(phil_id){
  int right_neighbor = phil_to_neighbor(phil_id, right);
  int left_neighbor = phil_to_neighbor(phil_id, left);

	if (phil_states[left_neighbor] != 1 && phil_states[right_neighbor] != 1 && phil_states[phil_id] == 0){
			phil_states[phil_id] = 1;
			pthread_cond_signal(&phil_waits[phil_id]);
	}
	
}

/*
 * Uses pickup_left_chopstick and pickup_right_chopstick
 * to pick up the chopsticks
 */   
void pickup_chopsticks(int phil_id){

 
	pthread_mutex_lock(&done_mutex);

	phil_states[phil_id] = 0;
	check_neighbors(phil_id);

	while (phil_states[phil_id] != 1) {
			pthread_cond_wait(&phil_waits[phil_id], &done_mutex);
	}
	
	pickup_right_chopstick(phil_id);
  pickup_left_chopstick(phil_id);

  pthread_mutex_unlock(&done_mutex);

}

/*
 * Uses pickup_left_chopstick and pickup_right_chopstick
 * to pick up the chopsticks
 */   
void putdown_chopsticks(int phil_id){
  int right_neighbor, left_neighbor;
  right_neighbor = phil_to_neighbor(phil_id, right);
  left_neighbor = phil_to_neighbor(phil_id, left);
  
  pthread_mutex_lock(&done_mutex);

  putdown_right_chopstick(phil_id);
  putdown_left_chopstick(phil_id);

	phil_states[phil_id] = 2;
	check_neighbors(right_neighbor);
	check_neighbors(left_neighbor);

  pthread_mutex_unlock(&done_mutex);
  
}


