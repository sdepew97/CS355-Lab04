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

int main(int argc, char *argv[]) {
    ucontext_t uc;
    ucontext_t main;

    void *stack;

    getcontext(&uc);
    stack = malloc(STACKSIZE);
    uc.uc_stack.ss_sp = stack;
    uc.uc_stack.ss_size = STACKSIZE;
    uc.uc_stack.ss_flags = SS_DISABLE;
    sigemptyset(&(uc.uc_sigmask));
    uc.uc_link = &main;

    makecontext(&uc, f, 0);
    setcontext(&uc);
    perror("setcontext"); //setcontext() does not return on success
    printf("Back in main\n");
    return 0;
}

//#include <stdio.h>
//#include <ucontext.h>
//
//static ucontext_t ctx[3];
//
//
//static void
//f1 (void) {
//    puts("start f1");
//    swapcontext(&ctx[1], &ctx[2]);
//    puts("finish f1");
//}
//
//static void
//f2 (void) {
//    puts("start f2");
//    swapcontext(&ctx[2], &ctx[1]);
//    puts("finish f2");
//}
//
//
//int
//main (void) {
//    char st1[8192];
//    char st2[8192];
//
//
//    getcontext(&ctx[1]);
//    ctx[1].uc_stack.ss_sp = st1;
//    ctx[1].uc_stack.ss_size = sizeof st1;
//    ctx[1].uc_link = &ctx[0];
//    makecontext(&ctx[1], f1, 0);
//
//
//    getcontext(&ctx[2]);
//    ctx[2].uc_stack.ss_sp = st2;
//    ctx[2].uc_stack.ss_size = sizeof st2;
//    ctx[2].uc_link = &ctx[1];
//    makecontext(&ctx[2], f2, 0);
//
//
//    swapcontext(&ctx[0], &ctx[2]);
//    printf("hello world\n");
//    return 0;
//}