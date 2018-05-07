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


struct process_file {
  struct file *file;
  int fd;
  struct list_elem elem;
};


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
	printf("\n ------------- system call: %d \n", call);
  //printf("system call halt %d!\n", SYS_EXIT);

	// hex_dump(f->esp, f-> esp, (int) (PHYS_BASE - f->esp), true);
	
  		//make a copy of the stack pointer


  void* arg[5];
  

  switch(call){
    case SYS_HALT:                   /* Halt the operating system. */
    {
      printf("-------------------------- SYSCALL HALT \n");
      shutdown_power_off();
    }

    case SYS_EXIT:                  /* Terminate this process. */
    {
      printf("-------------------------- SYSCALL.C EXITING \n");
      struct thread *cur = thread_current();
      printf ("%s: exit(%d)\n", cur->name, SYS_EXIT);
      thread_exit();
      exit(SYS_EXIT);
    }
    case SYS_EXEC:
    {
      printf("-------------------------- SYSCALL EXEC \n");
    }
    case SYS_WAIT:
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
    // printf("hello there read... \n");
    } 
    case SYS_WRITE: 
    {
    get_arg(f, &arg[0], 3);
    // printf("%s\n", "We have a write");  
    check_valid_buffer((void * ) arg[1], *(unsigned*) arg[2]);
    f->eax = write(*(int*)arg[0], *(char**) arg[1], *(unsigned *) arg[2]);
    break;
    }
                 /* Write to a file. */
    case SYS_SEEK:                   /* Change position in a file. */
    case SYS_TELL:                   /* Report current position in a file. */
    case SYS_CLOSE:                  /* Close a file. */

    /* Project 3 and optionally project 4. */
    case SYS_MMAP:                   /* Map a file into memory. */
    case SYS_MUNMAP:                 /* Remove a memory mapping. */

    default:
    // printf ("system call!\n");
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
  printf("--------------------------------------------- added child process \n");

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
    printf("-------------------- get child process \n");

     return cp;
   }
 }
 return NULL;
}
















void process_close_file (int fd)
{
  struct thread *t = thread_current();
  struct list_elem *next, *e = list_begin(&t->file_list);

  while (e != list_end (&t->file_list))
    {
      next = list_next(e);
      struct process_file *pf = list_entry (e, struct process_file, elem);
      if (fd == pf->fd || fd == -1)
  {
    file_close(pf->file);
    list_remove(&pf->elem);
    free(pf);
    if (fd != -1)
      {
        return;
      }
  }
      e = next;
    }
}





void remove_child_process (struct child_process *cp)
{

  list_remove(&cp->elem);
  free(cp);
}

void remove_child_processes (void)
{
  struct thread *t = thread_current();
  struct list_elem *next, *e = list_begin(&t->child_list);

  while (e != list_end (&t->child_list))
    {
      next = list_next(e);
      struct child_process *cp = list_entry (e, struct child_process,
               elem);
      list_remove(&cp->elem);
      free(cp);
      e = next;
    }
  printf("----------- syscall.c --- removed child processes \n");
}