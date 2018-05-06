#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"

#include <user/syscall.h>
#include "devices/input.h"
#include "devices/shutdown.h"
#include "filesys/file.h"
#include "filesys/filesys.h"
#include "threads/interrupt.h"
#include "threads/malloc.h"
#include "threads/synch.h"
#include "threads/thread.h"
#include "threads/vaddr.h"
#include "userprog/pagedir.h"
#include "userprog/process.h"

static void syscall_handler (struct intr_frame *);
void get_arg (struct intr_frame *f, int *arg, int n);
void check_valid_ptr (const void *vaddr);

void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

static void
syscall_handler (struct intr_frame *f UNUSED) 
{
  printf("SYSTEM CALL \n");
  int arg[2];
  check_valid_ptr((const void*) f->esp);
  switch (* (int *) f->esp)   {
    case SYS_HALT:	{
		halt(); 
		break;
	}

	case SYS_EXEC: {
		get_arg(f, &arg[0], 1);
		//arg[0] = user_to_kernel_ptr((const void *) arg[0]);
		//f->eax = exec((const char *) arg[0]); 
		break;
	}

	}
}

void halt (void)
{
  shutdown_power_off();
}



void get_arg (struct intr_frame *f, int *arg, int n)
{
  int i;
  int *ptr;
  for (i = 0; i < n; i++)
    {
      ptr = (int *) f->esp + i + 1;
      check_valid_ptr((const void *) ptr);
      arg[i] = *ptr;
    }
}

void check_valid_ptr (const void *vaddr)
{
  if (!is_user_vaddr(vaddr) || vaddr < ((void *) 0x08048000))
    {
      // exit(-1);
    }
}