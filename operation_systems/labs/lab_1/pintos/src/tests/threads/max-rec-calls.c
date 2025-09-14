/*
  File for 'max-rec-calls' task implementation.
*/

#include <stdio.h>
#include "tests/threads/tests.h"
#include "threads/malloc.h"
#include "threads/thread.h"
#include <stdlib.h>

void rec_func(int calls_amount, int i){
	calls_amount++;
	printf("Amount of calls: %d\n", calls_amount);
	if (calls_amount < 1000) rec_func(calls_amount, i);
}


void test_max_rec_calls(void) 
{
 //msg("Not implemented.");
	int calls_amount = 0;
	rec_func(calls_amount, 1);
	
}
