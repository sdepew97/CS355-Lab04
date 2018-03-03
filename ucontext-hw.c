#include <ucontext.h> 
#include <sys/types.h> 
#include <signal.h> 
#include <stdio.h> 
#include <unistd.h> 
#include <stdlib.h> 
#define STACKSIZE 4096 

void f(){
  printf("Hello World\n");
}

int main(int argc, char *argv[]){
  ucontext_t uc;
  void *stack;

  getcontext(&uc);
  stack = malloc(STACKSIZE);
  uc.uc_stack.ss_sp = stack;
  uc.uc_stack.ss_size = STACKSIZE;
  uc.uc_stack.ss_flags = SS_DISABLE;
  sigemptyset(&(uc.uc_sigmask));
  uc.uc_link = NULL;
  
  makecontext(&uc, f, 0);
  setcontext(&uc);
  perror("setcontext"); //setcontext() doesn’t return on success
  return 0;
}
