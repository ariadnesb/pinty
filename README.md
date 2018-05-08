# CMSC326-Pintos
Design Document for Project #2: System Calls and User Programs

## Group

Will Burghard <wb5289@bard.edu>

Jason Chang <jc8745@bard.edu>

Ariadne Sinnis-Bourozakis<as8594@bard.edu>

## PRELIMINARIES

Section 1: Argument Passing

### DATA STRUCTURES 

-For the argument passing section, we didn’t create any new structs, typedefs, or enumerations.

### ALGORITHMS 

Q1. 

Arguments are passed into our argument array using the strtok_r, which when iterated over breaks a continuous string into an array of strings as separated by a delimiter, which in our case is simply a space.
for(token = strtok_r((char *)fn_copy, “ “, &save_ptr); token != null; token = strtok_r(NULL, “ “, &save_ptr){
t[i] = token;
i += 1;
}
The arguments are written onto the stack using memcpy(), starting with the last argument, going one by one up the list of arguments and then writing the name of the program itself. 
The stack pointer is then adjusted to reach the word boundary of 4 bytes so as to prepare for the allocation of pointers to the arguments on the stack. Then pointers to each of the arguments on the stack are allocated, ending with a pointer to the program name.
Q2. The advantage of using the shell rather than the kernel to parse commands is that the shell is able to do more sophisticated pre-processing, acting as an interpreter rather than a simple messenger. For example, the shell can check whether or not the executable is really there before passing it to the kernel so as to avoid a kernel failure.

## Section 2: System Calls

### DATA STRUCTURES 

(In userprog/syscall.c):#define virt_bottom ((int *) 0x0804ba68)
Used in check_pointer to represent the bottom of user space in memory.
(In userprog/process.h): struct pfile {
                          struct file *file;
                          int fd;
                          struct list_elem pfelem;
                          };
Struct pfile is used exclusively in open() to store information about each file in a process. The pfile stores the file struct, file descriptor, and list_elem element of the file.
(In userprog/process.c) struct child_process {
  				int pid;
                        int load; 
  				bool wait;
  				bool exit;
  				int status;
		            struct lock wait_lock; 
  				struct list_elem elem;
};
Contains information about a child process as used in process_wait(). This information includes its pid, wait status, exit status, and list_elem element.

Q3. File descriptors, at least in Unix-like systems, are unique to each process. When a process opens a file, it designates its own file descriptor for that file and records it in a process-unique file table.

### ALGORITHMS 

Q4. We use separate functions for checking individual pointers and checking full buffers in memory, the latter function calling the former. The pointer function is fairly simple:
void check_pointer (const void *vadd){
    if (!is_user_vaddr(vadd) || vadd < virt_bottom){
   	 exit(ERROR);
    }
}
The buffer function takes a pointer to a buffer as well as the size of the buffer, then iterates through the buffer in memory, moving 4 bytes at a time and using check_pointer to validate every chunk of memory in the buffer:
void check_valid_buffer (void* buffer, unsigned size){
    unsigned i;
    char* local_buffer = (char *) buffer;
    for (i = 0 ; i<size; i++){
   	 check_pointer((const void *) local_buffer);
   	 local_buffer++;
    }
}
The memory validations occur right before the system call functions are called, after the arguments have been retrieved by get_arg.

Q5. How does the implementation of your wait system call work?  How does it interact with related processes and process termination?

The wait system call is implemented in the function process_wait. A child_process struct is used to represent the child’s status, and the parent contains a list of all children the parent has. The child process is initialized onto the parent’s child_list for management. Before the thread waits, the parent thread first checks to make sure it is not already waiting for a child process, and that there exists child process.

### SYNCHRONIZATION 

Q6. The exec system call returns -1 if loading the new executable
fails, so it cannot return before the new executable has completed loading.  How does your code ensure this?  How is the load success/failure status passed back to the thread that calls exec?
We never implemented exec, so this isn’t present in our code. If we were to do this I think the proper way is to hold a variable in the parent process which indicates the status of the child’s load, so that the load status is known regardless of whether or not the child has succeeded or failed.

Q7. Consider parent process P with child process C.  How do you  ensure proper synchronization and avoid race conditions when P calls wait(C) before C exits?  After C exits?  How do you ensure that all resources are freed in each case?  How about when P terminates without waiting, before C exits?  After C exits?

By using a child structure, we are able to represent each child process’ status. The list of child structures are used by the parent to prevent race conditions. Each child is responsible for updating its own status.
When P calls wait(C) before C exits, P checks first that the child process exists. 
When P calls wait(C) after C exits, P acquires the child and checks its exit status.
P terminates without waiting before C exists, the list inside P is freed. If C determines the parent has exited, it continues.
If P terminates after C exits the parent process releases all of its resources.




