
/*
  File for 'test-new-alg' task implementation.
*/

#include <stdio.h>
#include "tests/threads/tests.h"
#include "threads/malloc.h"
#include "threads/thread.h"
#include "threads/synch.h"
#include "devices/timer.h"

int elapsed;
void run_threads() {
	struct thread* t = thread_current();
	int start_time = timer_ticks();
	int worked_time;
	int prev_worked_time;
	worked_time = timer_ticks() - start_time;
	prev_worked_time = 0;
	while (true) {
		/*while (worked_time == prev_worked_time) {
			worked_time = timer_ticks() - start_time;
		}
		prev_worked_time = worked_time;*/
		//msg("thread = %s, ticks = %d", t->name, worked_time);
		/*if (worked_time == t->burst) {
			msg("thread %s is dying, burst = %d", t->name, t->burst);
			t->need_to_die = 1;
			thread_yield();
		}*/
	}
}


void test_new_alg(void) 
{
	thread_set_priority(PRI_MAX);
	test_new_alg_test = 1;
	thread_create("proc0", 27, run_threads, 3); //27
	msg("proc 0 created");
	thread_create("proc1", 7, run_threads, 8); //7
        msg("proc 1 created");
	thread_create("proc2", 8, run_threads, 20); //8
        msg("proc 2 created");
	thread_create("proc3", 18, run_threads, 6); //18
        msg("proc 3 created\n");
	thread_set_priority(PRI_MIN);
}
