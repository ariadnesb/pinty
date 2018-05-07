#ifndef USERPROG_SYSCALL_H
#define USERPROG_SYSCALL_H
#define ERROR -1

void syscall_init (void);
void check_valid_buffer (void* buffer, unsigned size);
void check_valid_ptr (const void *vaddr);
void exit (int status);


#endif /* userprog/syscall.h */

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