#ifndef USERPROG_PROCESS_H
#define USERPROG_PROCESS_H
int exit_stat;
#include "threads/thread.h"

struct ThreadsList {
	struct thread* t;
	struct ThreadsList* next;
	struct threadsList* prev;
};

tid_t process_execute (const char *file_name);
int process_wait (tid_t);
void process_exit (void);
void process_activate (void);

#endif /* userprog/process.h */
