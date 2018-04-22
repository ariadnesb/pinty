#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"

static void syscall_handler (struct intr_frame *);

void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

static void
syscall_handler (struct intr_frame *f UNUSED) 
{
  int call = *(int *)f->esp;
  char** arguments;
  void *esp_copy = f->esp;
  switch(call){
  	case(SYS_WRITE)
  		for(int i = 0; i < 3; i++){
  			esp_copy++;
  			arguments[i] = *(char*)esp_copy;
  			printf("%s", *arguments[i]);
  		
  	}
  }
  printf ("system call!\n");
  printf("%d", call);
  thread_exit ();
}
