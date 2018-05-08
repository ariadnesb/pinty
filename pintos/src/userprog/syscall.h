#ifndef USERPROG_SYSCALL_H
#define USERPROG_SYSCALL_H

#include "threads/synch.h"

#define CLOSE_ALL -1
#define ERROR -1

<<<<<<< HEAD
#define NOT_LOADED 0
#define LOAD_SUCCESS 1
#define LOAD_FAIL 2

=======
/*
>>>>>>> b3e781bfb3d92d2829595657fa0ab0792c2baf66
struct child_process {
  int pid;
  int load;
  bool wait;
  bool exit;
  int status;
  struct lock wait_lock;
  struct list_elem elem;
};

<<<<<<< HEAD
struct child_process* add_child_process (int pid);
struct child_process* get_child_process (int pid);
void remove_child_process (struct child_process *cp);
void remove_child_processes (void);
=======


does this help, I dont knw? also why do they sometimes not print if anyone could explain that to me
>>>>>>> b3e781bfb3d92d2829595657fa0ab0792c2baf66

void process_close_file (int fd);

void syscall_init (void);

#endif /* userprog/syscall.h */
