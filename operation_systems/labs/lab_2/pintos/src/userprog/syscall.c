#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "threads/vaddr.h"
#include "threads/init.h"
static void syscall_handler (struct intr_frame *);
is_filelist_created = 0;
struct list FilesList;
void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}


bool is_pointer_incorrect(const void* esp) {
	return !is_user_vaddr(esp) || pagedir_get_page(thread_current()->pagedir, esp) == NULL;
}


void halt(void) {
	shutdown_power_off();
}

void exit(int exit_code) {
	exit_output = exit_code;
	thread_exit();
}

tid_t exec(const char *cmd) {
	char* test_cmd = (char*)malloc(sizeof(char) * (strlen(cmd) + 1));
	char* temp;
	char* token;
	strlcpy(test_cmd, cmd, PGSIZE);
	token = strtok_r(test_cmd, " ", &temp);
	struct file* file = filesys_open(token);
	if (file == NULL) {
		printf ("load: %s: open failed\n", token);
		exit_output = -1;
		return -1;
	}
  	file_close(file);

	tid_t pid = process_execute(cmd);
	return pid;
}

bool create(const char *file, unsigned initial_size) { 
	if (is_pointer_incorrect((const char*)file)) exit(-1);
	return filesys_create(file, initial_size); 
}

int wait(tid_t pid) {
	return process_wait(pid);
}


int open(const char *file_name) {
	if (is_pointer_incorrect((const char*)file_name)) exit(-1);
	if (!is_filelist_created) {
		list_init(&FilesList);
		is_filelist_created = 1;
	}

	struct file* file = filesys_open(file_name);
	if (file == NULL) return -1;
	struct thread* thr = thread_current();
	struct files* new_file = (struct files*)malloc(sizeof(struct files));
	int descriptor = list_size(&FilesList) + 2;
	new_file->fd = descriptor;
	new_file->file = file;
	new_file->thread_id = thr->tid;
	list_push_back(&FilesList, &new_file->FileElem);
	return descriptor;
}


void close(int fd) {
	if (fd == 1 || fd == 0) exit(-1);
	else {
		struct files* found;
		struct list_elem* pointer = list_begin(&FilesList);
		while (pointer != list_tail(&FilesList)) {
			if (pointer == NULL) return;
			found = list_entry(pointer, struct files, FileElem);
			if (found->fd == fd) {
				file_close(found->file);
				list_remove(&found->FileElem);
			}
			pointer = list_next(pointer);
		}


	}
}

int remove(const char *file) {
	if (is_pointer_incorrect((const char*)file)) exit(-1);
	filesys_remove(file);
}

int filesize(int fd) {
	struct files* found;
        struct list_elem* pointer = list_begin(&FilesList);
        while (pointer != list_tail(&FilesList)) {
        	if (pointer == NULL) return;
                found = list_entry(pointer, struct files, FileElem);
                if (found->fd == fd) {
                        return file_length(found->file);
                }
                pointer = list_next(pointer);
        }
}


int read (int fd, void *buffer, unsigned size) {
	if (fd == 0) {
		char* new_buffer = (char*) buffer;
		//buffer = (char*) buffer;
		int i;
		for (i = 0; i < size; i++) {
			new_buffer[i] = input_getc();
		}
		return size;
	}
	struct files* found;
        struct list_elem* pointer = list_begin(&FilesList);
        while (pointer != list_tail(&FilesList)) {
                if (pointer == NULL) return -1;
                found = list_entry(pointer, struct files, FileElem);
                if (found->fd == fd) {
                        int to_return = file_read(found->file, buffer, size);
			return to_return;
                }
                pointer = list_next(pointer);
        }
}


int write (int fd, const void *buffer, unsigned size) {
	if (fd == 1) {
		putbuf(buffer, size);
		return size;
	}
	struct files* found;
        struct list_elem* pointer = list_begin(&FilesList);
        while (pointer != list_tail(&FilesList)) {
                if (pointer == NULL) return -1;
                found = list_entry(pointer, struct files, FileElem);
                if (found->fd == fd) {
                        int to_return = file_write(found->file, buffer, size);
                        return to_return;
                }
                pointer = list_next(pointer);
        }
}


static void
syscall_handler (struct intr_frame *f UNUSED) 
{

	if (is_pointer_incorrect((const void*)f->esp)) exit(-1);
	int nr = *(int*) f->esp;
	int* args = (int*)f->esp + 1;


	switch(nr) {
		//case SYS_WRITE:
		//	putbuf( ((const char**) f->esp)[2], ((size_t*) f->esp)[3]);
		//	break;
		case SYS_EXIT:
			exit(args[0]);
			break;
		case SYS_HALT:
			halt();
			break;
		case SYS_EXEC:
			if (is_pointer_incorrect((const char*) args[0])) exit(-1);
			f->eax = exec((const char*)args[0]);
			break;
		case SYS_CREATE:
			if (is_pointer_incorrect((const char*) args[0])) exit(-1);
			//if (args[1] == NULL) exit(-1);
			f->eax = create((const char*) args[0], args[1]);
			break;
		case SYS_WAIT:
			//if (is_pointer_incorrect((const char*) args[0])) exit(-1);
			f->eax = wait(args[0]);
			break;
		case SYS_REMOVE:
			if (is_pointer_incorrect((const char*) args[0])) exit(-1);
			f->eax = remove((const char*) args[0]);
			break;
		case SYS_OPEN:
			if (is_pointer_incorrect((const char*) args[0])) exit(-1);
			f->eax = open((const char*) args[0]);
			break;
		case SYS_CLOSE:
//			if (is_pointer_incorrect((const char*) args[0])) exit(-1);
			close(args[0]);
			break;
		case SYS_READ:
			if (is_pointer_incorrect((const void*)args[1])) exit(-1);
			f->eax = read(args[0], (void*)args[1], (unsigned)args[2]);
			break;
		case SYS_FILESIZE:
			f->eax = filesize(args[0]);
			break;
		case SYS_WRITE:
			if (is_pointer_incorrect((const void*)args[1])) exit(-1);
			f->eax = write(args[0], (void*)args[1], (unsigned)args[2]);
			break;


	}


    /*if ( *(int*) f->esp == SYS_WRITE) {
	    putbuf( ((const char**) f->esp)[2], ((size_t*) f->esp)[3]);

    }
    else if (*(int*) f->esp == SYS_EXIT) {
        exit_output = ((size_t*) f->esp)[1];
        thread_exit();
    }*/
    return;
}
