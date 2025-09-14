/*
  File for 'threads-pause-resume' task implementation.
*/

#include <stdio.h>
#include <stdlib.h>
#include <debug.h>
#include <stddef.h>
#include "tests/threads/tests.h"
#include "threads/malloc.h"
#include "threads/thread.h"
#include "threads/interrupt.h"
#include "threads/synch.h"
#include "devices/timer.h"
#include "threads/thread.h"
#include "threads/init.h"
#include "/home/rvevau/Downloads/pintos/src/threads/thread.h"


void thread_func_1(void *aux UNUSED) {
    while (1) {
        printf("Thread Thread_1 is running\n");
        timer_sleep(100);  // Спим 1 секунду (100 тиков = 1 секунда)
    }
}

/* Функция, которая будет выполняться во втором потоке */
void thread_func_2(void *aux UNUSED) {
    while (1) {
        printf("Thread Thread_2 is running\n");
        timer_sleep(100);  // Спим 1 секунду (100 тиков = 1 секунда)
    }
}

void test_threads_pause_resume(void){
	printf("\n\n\nStarting test...\n");
	 tid_t tid_1 = thread_create("Thread_1", 31, thread_func_1, NULL);
	 tid_t tid_2 = thread_create("Thread_2", 31, thread_func_2, NULL);

	timer_sleep(400);
printf("\n\n\n");
printf("=====================\n");
printf("| Thread Management |\n");
printf("=====================\n");

	thread_pause(tid_1); 
	printf("\n\n\n");
	timer_sleep(400);
printf("\n\n\n");
printf("=====================\n");
printf("| Thread Management |\n");
printf("=====================\n");

	thread_resume(tid_1);
printf("\n\n\n");
	timer_sleep(400);
	
	printf("Finishing test...\n\n\n");
}	


