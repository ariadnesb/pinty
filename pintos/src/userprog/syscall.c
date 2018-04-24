#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "devices/shutdown.h"


static void syscall_handler (struct intr_frame *);

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


	
  		

  
  	 
  

	switch(call){
		case SYS_HALT:                   /* Halt the operating system. */
		{
			shutdown_power_off();
		}

    	case SYS_EXIT:                  /* Terminate this process. */

    	case SYS_EXEC:                   /* Start another process. */
    	case SYS_WAIT:                   /* Wait for a child process to die. */
   		case SYS_CREATE:                 /* Create a file. */
    	case SYS_REMOVE:                 /* Delete a file. */
    	case SYS_OPEN:                   /* Open a file. */
    	case SYS_FILESIZE:               /* Obtain a file's size. */
    	case SYS_READ:                   /* Read from a file. */
    	case SYS_WRITE:
    	printf("%s\n", "We have a write");  
    	int fd = *((int*)f->esp + 1);
    	void* buffer = (void *)*((int*)f->esp + 2);
    	unsigned size = *((unsigned*)f->esp + 3);
          

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
