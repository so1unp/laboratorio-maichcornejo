#include "types.h"
#include "user.h"
#include "uthreadlib.h"

/* Possible states of a thread; */
#define FREE        0x0
#define RUNNING     0x1
#define RUNNABLE    0x2
#define STACK_SIZE  8192
#define MAX_THREAD  8

struct thread {
  int  sp;                /* saved stack pointer */
  char stack[STACK_SIZE]; /* the thread's stack */
  int  state;             /* FREE, RUNNING, RUNNABLE */
};

typedef struct thread thread_t, *thread_p;

static thread_t all_thread[MAX_THREAD];
static int current_thread_index;

thread_p  current_thread;
thread_p  next_thread;

/* This function is defined in uthread_switch.S */
extern void thread_switch(void);

void 
thread_init(void)
{
  // main() is thread 0, which will make the first invocation to
  // thread_schedule().  it needs a stack so that the first thread_switch() can
  // save thread 0's state.  thread_schedule() won't run the main thread ever
  // again, because its state is set to RUNNING, and thread_schedule() selects
  // a RUNNABLE thread.
  current_thread = &all_thread[0];
  current_thread->state = RUNNING;
  current_thread_index=0;
}

void 
thread_schedule(void)
{
  
    thread_p t;
    int start_index = (current_thread_index + 1) % MAX_THREAD; // Calcula el indice de inicio buscar

    /* Find another runnable thread. */
    next_thread = 0;
    int i;
    for ( i = 0; i < MAX_THREAD; i++) {
        t = &all_thread[(start_index + i) % MAX_THREAD]; // Recorre el arreglo de forma circular
        if (t->state == RUNNABLE && t != current_thread) {
            next_thread = t;
            current_thread_index = (start_index + i) % MAX_THREAD; // Actualiza current_thread_index
            break;
        }
    }

    if (next_thread == 0 && current_thread->state == RUNNABLE) {
        /* The current thread is the only runnable thread; run it. */
        next_thread = current_thread;
    }

    if (next_thread == 0) {
        printf(2, "thread_schedule: no runnable threads\n");
        //exit();
        next_thread = &all_thread[0];
        
    }

    if (current_thread != next_thread) { /* switch threads? */
        next_thread->state = RUNNING;
        thread_switch();
    } else {
        next_thread = 0;
    }
}

void 
thread_create(void (*func)())
{
  thread_p t;

  for (t = all_thread; t < all_thread + MAX_THREAD; t++) {
    if (t->state == FREE) break;
  }
  t->sp = (int) (t->stack + STACK_SIZE);   // set sp to the top of the stack
  t->sp -= 4;                              // space for return address
  * (int *) (t->sp) = (int)func;           // push return address on stack
  t->sp -= 32;                             // space for registers that thread_switch expects
  t->state = RUNNABLE;
}

void 
thread_yield(void)
{
  current_thread->state = RUNNABLE;
  thread_schedule();
}

void
thread_exit(void)
{
    current_thread->state = FREE;
    thread_schedule();
}

int
thread_id(void)
{
    return (int) current_thread;
}

