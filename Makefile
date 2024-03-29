CC = gcc            # default is CC = cc
CFLAGS = -g -Wall   # default is CFLAGS = [blank]

GTTHREADS_SRC = gtthread_sched.c gtthread_mutex.c steque.c
GTTHREADS_OBJ = $(patsubst %.c,%.o,$(GTTHREADS_SRC))

# pattern rule for object files
%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

#### Producer-Consumer ####
producer_consumer: producer_consumer.o steque.o
	$(CC) -o producer_consumer producer_consumer.o steque.o -lpthread

#### Dining ####
dining_main: dining_main.o chopsticks.o philosopher.o
	$(CC) -o dining_main $^ -lpthread

#### GTThreads ####
gtthread_main: gtthread_main.o $(GTTHREADS_OBJ)
	$(CC) -o gtthread_main gtthread_main.o $(GTTHREADS_OBJ)

#### Tennis ####
tennis: tennis.o tennis.c
	$(CC) -o tennis tennis.o

clean:
	$(RM) -f *.o producer_consumer dining_main gtthread_main tennis
