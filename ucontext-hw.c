//#include <ucontext.h>
//#include <sys/types.h>
//#include <signal.h>
//#include <stdio.h>
//#include <unistd.h>
//#include <stdlib.h>
//#define STACKSIZE 4096
//
//void f(){
//  printf("Hello World\n");
//}
//
//int main(int argc, char *argv[]) {
//    ucontext_t uc;
//    ucontext_t main;
//
//    void *stack;
//
//    getcontext(&uc);
//    stack = malloc(STACKSIZE);
//    uc.uc_stack.ss_sp = stack;
//    uc.uc_stack.ss_size = STACKSIZE;
//    uc.uc_stack.ss_flags = SS_DISABLE;
//    sigemptyset(&(uc.uc_sigmask));
//    uc.uc_link = &main;
//
//    makecontext(&uc, f, 0);
////    setcontext(&uc);
////    perror("setcontext"); //setcontext() does not return on success
//    swapcontext(&main, &uc);
//    printf("Back in main\n");
//    return 0;
//}
//
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
//    ctx[1].uc_link = NULL; //was &ctx[0]
//    makecontext(&ctx[1], f1, 0);
//
//
//    getcontext(&ctx[2]);
//    ctx[2].uc_stack.ss_sp = st2;
//    ctx[2].uc_stack.ss_size = sizeof st2;
//    ctx[2].uc_link = &ctx[1];
//    makecontext(&ctx[2], f2, 0);
//
//    //swapcontext(&ctx[0], &ctx[2]);
//    setcontext(&ctx[1]);
//    printf("hello world\n");
//    return 0;
//}

//#include <stdio.h>
//#include <ucontext.h>
//#include <unistd.h>
//
//int main(int argc, const char *argv[]){
//    ucontext_t main;
//    printf("start\n");
//    getcontext(&main);
//    puts("Hello world\n");
//    sleep(1);
//    setcontext(&main);
//    printf("end");
//    return 0;
//}

//3. What happens when a signal is delivered? How do signals affect the execution of a context?
//#include <stdio.h>
//#include <ucontext.h>
//#include <unistd.h>
//
//int main(int argc, const char *argv[]){
//    ucontext_t main;
//    alarm(10);
//    printf("start\n");
//    getcontext(&main);
//    puts("Hello world\n");
//    sleep(1);
//    setcontext(&main);
//    printf("end");
//    return 0;
//}

//4. Can you write a program that alternates between two functions at a set interval? Recall the kernel timer lab we did.

/*
	File:	kt_frame.c
	Frame for the kernel timer lab exercise
*/

#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <signal.h>
#include <stdlib.h>
#include <ucontext.h>

#define INTERVAL_SECS 		1
#define INTERVAL_MICROSECS 	0
#define VALUE_SECS 		1
#define VALUE_MICROSECS 	0
#define	MICROS 			1000000

int numSwitches;

void setrtimer(struct itimerval *);
void catchsig(int);
void showtime(struct itimerval *);
void age(const struct timeval *);
static void f1 (void);
static void f2 (void);
static ucontext_t ctx[3];

int main(int argc, char **argv) {
    struct itimerval realt;
    struct timeval start;

    char st1[8192];
    char st2[8192];

    if (argc > 1) {
        printf("Usage: %s takes no arguments, ctrl-c to quit\n", argv[0]);
        exit(0);
    }

    gettimeofday(&start, NULL);
    setrtimer(&realt);

    if (setitimer(ITIMER_REAL, &realt, NULL) == -1) {
        perror("error calling setitimer()");
        exit(1);
    }

    //send signal once every second...
    signal(SIGALRM, catchsig);

    getcontext(&ctx[1]);
    ctx[1].uc_stack.ss_sp = st1;
    ctx[1].uc_stack.ss_size = sizeof st1;
    ctx[1].uc_link = NULL; //was &ctx[0]
    makecontext(&ctx[1], f1, 0);


    getcontext(&ctx[2]);
    ctx[2].uc_stack.ss_sp = st2;
    ctx[2].uc_stack.ss_size = sizeof st2;
    ctx[2].uc_link = &ctx[1];
    makecontext(&ctx[2], f2, 0);

    swapcontext(&ctx[0], &ctx[1]);
//    sleep(1);

    return 0;
}


static void f1 (void) {
    while(1) {
        printf("function 1\n");
        pause();
    }
}

static void f2 (void) {
    while(1) {
        puts("function 2\n");
        pause();
    }
}

/*
  Initialize the ITIMER_REAL interval timer.
  Its interval is one second.  Its initial value is one second.
*/
void setrtimer(struct itimerval *ivPtr) {
    ivPtr->it_interval.tv_sec = INTERVAL_SECS;
    ivPtr->it_interval.tv_usec = INTERVAL_MICROSECS;
    ivPtr->it_value.tv_sec = VALUE_SECS;
    ivPtr->it_interval.tv_usec = VALUE_MICROSECS;
}

/*
  Define a signal handler for a real interval timer.
  Increment the global realSeconds;
  Reset the handler for SIGALRM.
*/
void catchsig(int sig) {
    numSwitches++;
    printf("caught SIGALARM\n");
    if(numSwitches%2==0) {
        swapcontext(&ctx[2], &ctx[1]);
    }
    else {
        swapcontext(&ctx[1], &ctx[2]);
    }

    signal(SIGALRM, catchsig);
}
