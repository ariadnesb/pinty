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

#define false -1
#define true 1
#define virt_bottom ((int *) 0x0804ba68)
struct file * get_file(int fd);
bool create (const *file_name, unsigned start_size);
int check_valid_buffer (void* buffer, unsigned size);
int check_pointer (const void *vadd);
int user_to_kernel_pointer (const void * vadd);
int write(int fd, void* buffer, unsigned size);
void get_command_args (struct intr_frame *f, int *arg, int n);
void exit (int status);
int wait (int pid);
int open (const char * file);
struct child_process* add_child_process (int pid);
struct child_process* get_child_process (int pid);
struct file* get_file (int fd);



struct process_file {
  struct file *file;
  int fd;
  struct list_elem elem;
};


static void syscall_handler (struct intr_frame *);

void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

static void
syscall_handler (struct intr_frame *f UNUSED) 
{

	int call = *(int *) f-> esp; //make a copy of the stack pointer

	// printf("\n ------------- system call: %d \n", call);
  //printf("system call halt %d!\n", SYS_EXIT);
	// hex_dump(f->esp, f-> esp, (int) (PHYS_BASE - f->esp), true);

  void* arg[3]; //where the command arguments will be stored for get_command_args

  switch(call){
    case SYS_HALT:                   /* Halt the operating system. */
    {
      // // printf("-------------------------- HALT \n");
      shutdown_power_off();
      break;
    }

    case SYS_EXIT:                  /* Terminate this process. */
    {
      //// printf("-------------------------- EXITING \n");
      get_command_args(f, &arg[0], 1);
      thread_current ()->status = *(int*)arg[0];      
      thread_exit();
      break;
    }

    case SYS_EXEC:
    {
      // printf("-------------------------- EXEC \n");
      break;
    }

    case SYS_WAIT:
    {
      // printf("begin on sys_wait\n");
      get_command_args(f, &arg[0], 1); // Get the first argument
      f->eax = wait(arg[0]);
      break;
    }

    case SYS_CREATE:                 /* Create a file. */
    {
      get_command_args(f, &arg[0], 3);
      arg[0] = user_to_kernel_pointer((const void *) arg[0]);
      
      bool success = create(*(char**) (f->esp+4), *(int*) (f->esp +8));
      f->eax = success;
      return success;
      break;
    }

    case SYS_REMOVE:                 /* Delete a file. */
    {
      get_command_args(f, &arg[0], 1);
      arg[0] = user_to_kernel_pointer((const void *) arg[0]);
      bool success = filesys_remove((const char *) arg[0]);
      f->eax = success;
      break;
    }

    case SYS_OPEN:                   /* Open a file. */
    {
    	get_command_args(f, &arg[0], 1);
    	arg[0] = user_to_kernel_pointer((const void*) arg[0]);
      int success = open(*(char**) arg[0]);
      if (!success) return -1;
    	f->eax = success;
    	break;
    }

    case SYS_FILESIZE:               /* Obtain a file's size. */
    {
      get_command_args(f, &arg[0], 1);
      f->eax = get_file_length(*(int *) arg[0]);
      break;
    }

    case SYS_READ:                   /* Read from a file. */
    {
      return -1;
      break;
    }
    case SYS_WRITE:                  /* Write to a file. */
    {
      get_command_args(f, &arg[0], 3);
      // printf("%s \n", arg[1]);  
      if (!check_valid_buffer((void * ) arg[1], *(unsigned*) arg[2])){
        thread_exit();
      }
      f->eax = write(*(int*)arg[0], *(char**) arg[1], *(unsigned *) arg[2]);
      break;
    }
                 
    case SYS_SEEK:                   /* Change position in a file. */
    { 
      //honestly, this doesn't seem to do anything worthwile but I spent time on it so it's here
      get_command_args(f, &arg[0], 2);
      struct file *file = get_file(arg[0]);
      if (!file){
        break;
      }
      file_seek(file, (unsigned) arg[1]);
      break;
    }
      /* MASTER BRANCH
    	case SYS_EXEC:                  
    	case SYS_WAIT:                  
   		case SYS_CREATE:                
    	case SYS_REMOVE:        
    	case SYS_OPEN:               
    	case SYS_FILESIZE:        
    	case SYS_READ:               
    	case SYS_WRITE:
    	printf("%s\n", "We have a write");  
    	int fd = *((int*)f->esp + 1);
    	void* buffer = (void *)*((int*)f->esp + 2);
    	unsigned size = *((unsigned*)f->esp + 3);
    	//printf("%d\n", size);
    	f->eax = write(fd, buffer, size);
    	printf("\n");  END OF MASTER */
      
    case SYS_TELL:                   /* Report current position in a file. */
      break;

    case SYS_CLOSE:                  /* Close a file. */
    {
      return -1; //this is simply funny
      break;
    }

    /* Project 3 and optionally project 4. */
    case SYS_MMAP:                   /* Map a file into memory. */
    case SYS_MUNMAP:                 /* Remove a memory mapping. */

    default:
    // printf ("system call!\n");
    thread_exit ();
  }
}
  /*gets all the syscall arguments from the stack and casts 
  them all as void - they must later be cast to the correct types */
void get_command_args (struct intr_frame *f, int *arg, int n)
{
 int i;
 int *ptr;
 int wlen = sizeof(void *);
 for (i = 0; i < n; i++)
   {
     arg[i] =  f->esp + ((i + 1) *wlen);
   }
}

//makes sure the pointer is valid 
int check_pointer (const void *vadd){
  if (!is_user_vaddr(vadd) || vadd < virt_bottom){
    return -1;
  }
  return 1;
}

//goes through the buffer and makes sure all the memory is valid pointers
int check_valid_buffer (void* buffer, unsigned size){
	unsigned i;
	char* local_buffer = (char *) buffer; 
	for (i = 0 ; i<size; i++){
		if (!user_to_kernel_pointer((const void *) local_buffer)){ 
      return -1;
    }
    else{
		  local_buffer++;
    }
    return 1;
	}
}

int user_to_kernel_pointer (const void * vadd){
	if (!check_pointer(vadd)) return -1;
	void *pointer = pagedir_get_page(thread_current()->pagedir, vadd);
	if (!pointer){
		return -1;
	}
	return (int) pointer; //does this need to be derefrenced here?
}

void exit (int status){
  thread_exit();
}

// <<<<<<< jason-implementation-5-8
//this does not actually seem to work
int wait (int pid)
{
  return process_wait(pid);
}

/* writes to stdout if fd is 1, otherwise should write to the 
file but this does not seem to work*/
int write(int fd, void* buffer, unsigned size){
  if(fd == 1){
   putbuf(buffer, size);
   return (0);
 }
 else if (get_file(fd) != NULL){
 	struct file *f = get_file(fd);
 	int byteswritten= file_write(f, buffer, size);
 	return byteswritten;
 }
 return (-1);
}

bool create (const *fname, unsigned size){
  return filesys_create(fname, size);
/*

}

int write(int fd, void* buffer, unsigned size){
	if(fd == 1){
    		putbuf(buffer, size);
    	}
    else{

    }

bool check_ptr(void* ptr){
	
}
master*/
}

int open (const char * file){
  // loads and opens the file, then pushes the file onto the file list
  if (!file || file == NULL ) return -1;
	struct file *f =filesys_open(file);
  if (!f || f == NULL) return -1;
  check_pointer(f);
	struct pfile *process_file = malloc(sizeof(struct pfile));
	process_file -> fd = thread_current()-> fd +=1 ; 
	process_file -> file = f;
	list_push_back(&thread_current()-> file_list, &process_file->pfelem);
	return (process_file->fd);
}

int get_file_length(file)
{
	struct file *f = get_file(file);
    if (!f) return -1;
    int size =file_length(f);
    return size;
  }

struct child_process* add_child_process (int pid)
{
  // Add child process to parent child process' list
  struct child_process* cp = malloc(sizeof(struct child_process));
  cp->pid = pid;
  cp->load = NOT_LOADED;
  cp->wait = false;
  cp->exit = false;
  lock_init(&cp->wait_lock);
  // Push the lock onto the stack
  list_push_back(&thread_current()->child_list,
   &cp->elem);
  return cp;
}

struct child_process* get_child_process (int pid)
{
  // Get specific child process from parent child's list
  struct thread *t = thread_current();
  struct list_elem *e;

  // iterate through parent's child list and fetch pid
  for (e = list_begin (&t->child_list); e != list_end (&t->child_list);
   e = list_next (e))
  {

    struct child_process *cp = list_entry (e, struct child_process, elem);
    // return found child process
    if (pid == cp->pid)return cp;
  }
  return NULL;
}

//get file with specific fd from the file_list
struct file* get_file (int fd)
{
  struct thread *t = thread_current();
  struct list_elem *e;
  //iterate through and find the file with fd
  for (e = list_begin (&t->file_list); e != list_end (&t->file_list);
       e = list_next (e)){
          struct process_file *pf = list_entry(e, struct process_file, elem);
          if (fd == pf->fd) return pf->file;
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