<div id="table-of-contents">
<h2>Table of Contents</h2>
<div id="text-table-of-contents">
<ul>
<li><a href="#sec-1">1. GTThreads: A Preemptive User-level Threads Library</a>
<ul>
<li><a href="#sec-1-1">1.1. Goal</a></li>
<li><a href="#sec-1-2">1.2. Setup</a></li>
<li><a href="#sec-1-3">1.3. Collaboration</a></li>
<li><a href="#sec-1-4">1.4. Warm-up</a></li>
<li><a href="#sec-1-5">1.5. Dining Philosophers</a></li>
<li><a href="#sec-1-6">1.6. GTthreads</a></li>
<li><a href="#sec-1-7">1.7. Grading</a></li>
</ul>
</li>
</ul>
</div>
</div>

---

# GTThreads: A Preemptive User-level Threads Library<a id="sec-1" name="sec-1"></a>

## Goal<a id="sec-1-1" name="sec-1-1"></a>

The end goal of this project is to write a preemptive user-level threads library with mutex exclusion.  Along the way, you will become familiar with the pthreads library and solve the classic mutual exclusion puzzle of “The Dining Philosophers”.

## Setup<a id="sec-1-2" name="sec-1-2"></a>

1.  Gain access to a well-maintained linux machine.  If you do not have this already, then it is recommended that you follow the [instructions](https://www.udacity.com/wiki/ud156-virtualbox) for downloading and installing VirtualBox and the AOS Virtual Image.

2.  Clone this repository, which contains the starter code for the project.
    
        git clone git@github.gatech.edu:sb300/oms-aos-gtthreads.git

3.  You can confirm that you have pthreads installed on your linux platform with the command
    
        ldconfig -p | grep libpthread

---

## Collaboration<a id="sec-1-3" name="sec-1-3"></a>

For this and all projects, students must work on their own.

---

## Warm-up<a id="sec-1-4" name="sec-1-4"></a>

To familiarize yourself with the pthreads library, you will debug a simple test program named producer_consumer.c.  You may read about the [pthread api](https://computing.llnl.gov/tutorials/pthreads/) or consult the man pages on your virtual platform.  For example, to learn about pthread_join, type

    man pthread_join

in a terminal.

There are five bugs in total (you may consult the videos for the project to help you find them.)
After finding these bugs, provide answers to the questions given in the QA_producter_consumer.txt in that file.

---

## Dining Philosophers<a id="sec-1-5" name="sec-1-5"></a>

A good exercise for understanding the perils of deadlock and how to avoid them is the classic “Dining Philosophers” problem originally posed by Dijtkstra:

> There are five philosophers sitting around a table.  Being philosophers, they like to think, but they also have to eat every now and again.  Each philosopher gets his own rice bowl, but there only five chopsticks, one laid between each pair of neighboring philosophers.  When wishing to eat, a philosopher needs to pick up the two chopsticks on either side of him.  When wishing to think, he places the chopsticks back in their places for his neighbors to use.

The file dining.c contains starter code for the problem.  Your task is to complete the given program by filling in the code where indicated.  The output should read like directions for a play, where the action involves philosophers thinking,  picking up chopsticks, eating, and putting them down.

The directions for the play must be valid.  That is, a philosopher can only pick up a chopstick if it is available and only put one down if he has it.  Equally important is that no philosopher should be starved.  A particularly bad situation is deadlock, where all five philosophers pick up the chopstick to their left and wait for the one to their right to be set down.  Of course, this chopstick will never be set down because it is in the left hand of another philosopher.  In this deadlocked situation, the philosophers starve.  Take care that your program avoids this problem.

There is no requirement to implement fairness among the philosophers, as long as everyone gets to eat when chopsticks are available.  For instance, one way to avoid deadlock would be just to prevent one philosopher from ever picking up chopsticks.  Another is to use a single global mutex that only allows one philosopher to eat at time.  Neither of these are good solutions.

## GTthreads<a id="sec-1-6" name="sec-1-6"></a>

The main part of the project is to implement a user-level thread library with an api similar to POSIX threads or pthreads.  The specifications for implementing this api are found in the two files 
[gtthread_sched.c](https://github.gatech.edu/sb300/oms-aos-gtthreads/blob/master/gtthread_sched.c) and [gtthread_mutex.c](https://github.gatech.edu/sb300/oms-aos-gtthreads/blob/master/gtthread_mutex.c)

The following strategies are highly recommended:

-   Use the provided steque data structure (in files steque.[ch]) for both your scheduling queue and your mutex locks.
-   Use swapcontext, makecontext, and getcontext functions, which are illustrated in the example program [tennis.c](https://github.gatech.edu/sb300/oms-aos-gtthreads/blob/master/tennis.c), to switch among your threads.
-   Use setitimer, sigaction, sigprocmask, and related functions, which are illustrated in the example program [defcon.c](https://github.gatech.edu/sb300/oms-aos-gtthreads/blob/master/defcon.c), to achieve pre-emption among your threads.  Consult this [post](http://www.linuxprogrammingblog.com/all-about-linux-signals?page=show) and the [wikipedia article](http://en.wikipedia.org/wiki/Unix_signal) for more information.

It is also suggested that you work incrementally.  For instance, you might the task into the following pieces:

1.  Implement and test the library without pre-emption or mutexes and use gtthread_yield only to change threads.
2.  Add preemption via the signal handler.
3.  Add the mutex capabilities.

---

## Grading<a id="sec-1-7" name="sec-1-7"></a>

<table border="2" cellspacing="0" cellpadding="6" rules="groups" frame="hsides">


<colgroup>
<col  class="left" />

<col  class="right" />
</colgroup>
<thead>
<tr>
<th scope="col" class="left">Deliverable</th>
<th scope="col" class="right">Grade</th>
</tr>
</thead>

<tbody>
<tr>
<td class="left">producer_consumer.c</td>
<td class="right">5%</td>
</tr>


<tr>
<td class="left">QA_producer_consumer.txt</td>
<td class="right">5%</td>
</tr>


<tr>
<td class="left">dining.c</td>
<td class="right">10%</td>
</tr>


<tr>
<td class="left">gtthreads library</td>
<td class="right">80%</td>
</tr>
</tbody>
</table>