#ifndef USERPROG_SYSCALL_H
#define USERPROG_SYSCALL_H
#define ERROR -1
#define NOT_LOADED 0


void syscall_init (void);

#endif /* userprog/syscall.h */

#include "threads/synch.h"

struct child_process* add_child_process (int pid);
struct child_process* get_child_process (int pid);
void remove_child_process (struct child_process *cp);

struct child_process {
  int pid;
  int load;
  bool wait;
  bool exit;
  int status;
  struct lock wait_lock;
  struct list_elem elem;
};

/*

does this help, I dont knw? also why do they sometimes not print if anyone could explain that to me

cr2 = control register
error = error
esa-dsx = general purpose register
esi = index register for ds
edi =  "      "       "  es
esp = stack pointer
ebp = access data on the stack
cs = code segment register : added to address during instruction 'fetch' apparentlu
ds = data segment register : added to the address when accessing a memory operand that is not on the stack 
es = extra segment register : also ised in special instructions that span segements ( cole: string cpies)
ss = stack segement, added to address during the stack access


*/