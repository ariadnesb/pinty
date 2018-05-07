#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "devices/shutdown.h"
#include "threads/vaddr.h"
#include "filesys/file.h"
#include "filesys/inode.h"
#include "threads/malloc.h"
#include "threads/synch.h"
#include "userprog/pagedir.h"
#include "userprog/process.h"

#define virt_bottom ((int *) 0x0804ba68)

static void syscall_handler (struct intr_frame *);
//void * arg[]

void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

static void
syscall_handler (struct intr_frame *f UNUSED) 
{

	int call = *(int *) f-> esp;
	printf("system call numero %d!\n", call);

	hex_dump(f->esp, f-> esp, (int) (PHYS_BASE - f->esp), true);
	
  		//make a copy of the stack pointer

  
  	 void* arg[5];
  

	switch(call){
		case SYS_HALT:                   /* Halt the operating system. */
		{
			shutdown_power_off();
		}

    	case SYS_EXIT:                  /* Terminate this process. */
			{
				thread_exit();
			}
    	case SYS_EXEC:                   /* Start another process. */
    	case SYS_WAIT:                   /* Wait for a child process to die. */
			{
				printf("begin on sys_wait\n");
				get_arg(f, &arg[0], 1);
				f->eax = wait(arg[0]);
				break;
			}
   		case SYS_CREATE:                 /* Create a file. */
    	case SYS_REMOVE:                 /* Delete a file. */
    	case SYS_OPEN:                   /* Open a file. */
    	case SYS_FILESIZE:               /* Obtain a file's size. */
    	case SYS_READ:                   /* Read from a file. */
    	{
    		printf("hello there read... \n");
    	} /*
    	case SYS_WRITE:                //  /* Write to a file. 
    	{
    		printf("shouldn't this be fd= 1??: %d\n", f->esp +1);
    		printf("writey\n");
    		//get_arg(f, &arg[0], 3);
			//check_valid_buffer((void *) arg[1], (unsigned) arg[2]);
			//arg[1] = user_to_kernel_ptr((const void *) arg[1]);
			//f->eax = write(arg[0], (const void *) arg[1],
		    //(unsigned) arg[2]);
			
			break;
      	} */
    	case SYS_WRITE: 
    	{
    	get_arg(f, &arg[0], 3);
    	printf("%s\n", "We have a write");  
    	//int fd = *((int*)f->esp + 1);
    	//void* buffer = (void *)*((char**)f->esp + 2);
    	//unsigned size = *((unsigned*)f->esp + 3);
    	//printf("%d , fd\n", fd);
    	check_valid_buffer((void * ) arg[1], *(unsigned*) arg[2]);
    	f->eax = write(*(int*)arg[0], *(char**) arg[1], *(unsigned *) arg[2]);
        break;
    	//printf("buff siz: %d\n", size);
    }
    	               /* Write to a file. */
    	case SYS_SEEK:                   /* Change position in a file. */
    	case SYS_TELL:                   /* Report current position in a file. */
    	case SYS_CLOSE:                  /* Close a file. */

    /* Project 3 and optionally project 4. */
    	case SYS_MMAP:                   /* Map a file into memory. */
    	case SYS_MUNMAP:                 /* Remove a memory mapping. */

		default:
			printf ("system call!\n");
  			thread_exit ();



	}
}

void check_valid_buffer (void* buffer, unsigned size){
	unsigned i;
	char* local_buffer = (char *) buffer; 
	for (i = 0 ; i<size; i++){
		check_valid_ptr((const void *) local_buffer);
		local_buffer++;
	}
}

void check_valid_ptr (const void *vaddr){
	if (!is_user_vaddr(vaddr) || vaddr < virt_bottom){
		exit(ERROR);
	}
}


	void get_arg (struct intr_frame *f, int *arg, int n)
	{
	  int i;
	  int *ptr;
	  int wlen = sizeof(void *);
	  for (i = 0; i < n; i++)
	    {
	      //ptr = (int *) f->esp + ((i + 1) *wlen);
	      //check_valid_ptr((const void *) ptr);
	      //arg[i] = *ptr;
	      arg[i] =  f->esp + ((i + 1) *wlen);
	    }
}

	int write(int fd, void* buffer, unsigned size){

		if(fd == 1){
	    		putbuf(buffer, size);
    }
}

void exit (int status){
	thread_exit();
}

int wait (int pid)
{
  return process_wait(pid);
}

struct child_process* add_child_process (int pid)
{
  struct child_process* cp = malloc(sizeof(struct child_process));
  cp->pid = pid;
  cp->load = NOT_LOADED;
  cp->wait = false;
  cp->exit = false;
  lock_init(&cp->wait_lock);
  list_push_back(&thread_current()->child_list,
		 &cp->elem);
  return cp;
}

struct child_process* get_child_process (int pid)
{
  struct thread *t = thread_current();
  struct list_elem *e;

  for (e = list_begin (&t->child_list); e != list_end (&t->child_list);
       e = list_next (e))
        {
          struct child_process *cp = list_entry (e, struct child_process, elem);
          if (pid == cp->pid)
	    {
	      return cp;
	    }
        }
  return NULL;
}

void remove_child_process (struct child_process *cp)
{
  list_remove(&cp->elem);
  free(cp);
}