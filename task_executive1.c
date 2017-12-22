/*
 File        : task_executive1.c
 Program     : INFO8110
 Programmer  : Rohit Bhardwaj
 Date        : 2016-11-30
 Description : This program works as a real co-operative multitasker
*/

//Header Files
#include <stdio.h>
#include <string.h>
#include "stm32f3xx_hal.h"
#include "stm32f3_discovery.h"
#include "task_executive1.h"
#include "common.h"

#define MAX_TASKS       10
#define TASK_STACK_SIZE 0x100

//Structure
typedef struct PCB_s 
{
/* Storage for r4-r14 */
 uint32_t r4;
 uint32_t r5;
 uint32_t r6;
 uint32_t r7;
 uint32_t r8;
 uint32_t r9;
 uint32_t sl;
 uint32_t fp;
 uint32_t ip;
 uint32_t lr;
 uint32_t sp; /* Note location...*/
} PCB_t;

typedef struct Task_s 
{
 void (*f)(void *data);                        /* Task function */
 void *data;                                   /* Private data pointer for this task */
 uint32_t *stack;                              /* Pointer to top of task's stack */
 PCB_t pcb;                                    /* Process control block */
}Task_t;

/* Space for process stacks. */
Task_t tasks[MAX_TASKS];                      //Array(task) to hold the list of currently active tasks
int32_t currentTask ;                         //Global Variable to keep track of the current task
uint32_t stacks[MAX_TASKS][TASK_STACK_SIZE];  //Array(stacks) to hold the stack


// Function     : TaskNull()
// Description  : This function will act as a default task so that the first 
//                call to TaskSwitcher() has somewhere to end up            
// Parameters   : void *data
// Returns      : None
static void TaskNull(void * data)
{
 while(1)
 {
 }
}


// Function     : TaskInit()
// Description  : This function Intializes the Task Executive              
// Parameters   : None
// Returns      : None
void TaskInit(void)
{
 for(int i = 0; i<MAX_TASKS; i++)
 {
  //Creating Empty Slots By filling the function 
  //pointer tasks[].f with NULL 
  tasks[i].f = NULL;
  tasks[i].data = NULL;
 }
  //Default task
  tasks[0].f = TaskNull;
  tasks[0].data = NULL;
  currentTask = 0;
}

// Function     : TaskKill()
// Description  : This function kills the Task              
// Parameters   : int32_t id
// Returns      : int32_t
int32_t TaskKill(int32_t id)
{
  if((id < 0) || (id >= MAX_TASKS))
  {
   printf("Out of Range\n");
   return -1;
  }

  if(tasks[id].f == NULL)
  {
   printf("\n ALREADY DEAD");
   return -1;
  }
  //Set the function pointer in the array entry at the passed 
  //id equal to NULL to make it inactive
  tasks[id].f    = NULL;
  tasks[id].data = NULL;
  return id;
}

//To avoid having the compiler generate any stack accesses at the entry 
//and exit of the function we use an attribute to make the function Naked

// Function     : TaskShell()
// Description  : This function calls the appropriate Task Function, after that
//                it kill that task and calls the scheduler and never returns            
// Parameters   : None
// Returns      : None
__attribute__((naked)) static void TaskShell(void)
{
 /* call the appropriate Task Function */
 tasks[currentTask].f(tasks[currentTask].data);
 /* Terminate this task */
 TaskKill(currentTask);
 /* Call scheduler, Never returns */
 TaskSwitcher();
}

// Function     : TaskAdd()
// Description  : This function adds the Task              
// Parameters   : function pointer and data
// Returns      : int32_t
int32_t TaskAdd(void (*f)(void *data), void *data)
{
 
 for(int i=0; i<MAX_TASKS; i++) 
 {
 memset(&(tasks[i].pcb),0,sizeof(tasks[i].pcb));
 tasks[i].pcb.sp =(uint32_t)(&(stacks[i][TASK_STACK_SIZE-4]));            //Initialize the stack pointer
 tasks[i].pcb.fp =(uint32_t)(&(stacks[i][TASK_STACK_SIZE-4]));            //Initialize the frame pointer
 tasks[i].pcb.lr =(uint32_t)TaskShell;                                    //Initialize the link register
 /* Put an initial stack frame on too */
 stacks[i][TASK_STACK_SIZE-1]=(uint32_t)(TaskShell);                      //On the newly allocated stack push a copy of the link register
 if(tasks[i].f == NULL)                                                   //check if the slot is empty
 {
 tasks[i].f = f;                                                          //Fill the function pointer with the pointer passed in
 tasks[i].data = data;
 tasks[i].stack = stacks[i];
 return i;                                                                //return the index(i)
 }
 }
 return -1;
}

// Function     : TaskNext()
// Description  : This function finds the next task to run           
// Parameters   : None
// Returns      : int32_t 
static int32_t TaskNext(void)
{
  int32_t i;
  uint32_t count=0;
  
  i = currentTask;
  do {
    /* NOTE: for this expression to work correctly, MAX_TASKS must be an EVEN number */
    //i = (i + 1) % MAX_TASKS;
    i = ((i + 1) % (MAX_TASKS-1))+1;
    count++;
  } while((tasks[i].f == NULL) && (count <= MAX_TASKS));

  return (count <= MAX_TASKS) ? i : -1;
}


//The trick here is that after the context switch, the Link Register 
//will hold the return address to the new task, and not the old one.

// Function     : TaskSwitcher()
// Description  : This function will run all the active tasks in sequence          
// Parameters   : None
// Returns      : int32_t
int32_t TaskSwitcher(void)
{
  int32_t nextTask;
  register PCB_t *currentPCB asm("r0");
  register PCB_t *nextPCB asm("r1");
   printf("Task switcher called\n");
  nextTask = TaskNext();
  
  if(nextTask < 0) 
  {
   /* In the case of no tasks to run,
   return to the original thread */
   printf("TaskSwitcher(): ""No tasks to run!\n");
   nextTask = 0;
  }
  /* If the current task is the only one
  to be running, just return */
  if(nextTask == currentTask)
  return 0;
  currentPCB = &(tasks[currentTask].pcb);
  nextPCB = &(tasks[nextTask].pcb);
  currentTask = nextTask;

  //Inline assembly is used to save and restore the contexts
 asm volatile (
  "stm %[current]!,{r4-r12,r14}\n\t"
  "str sp, [%[current]]\n\t"
  "ldm %[next]!,{r4-r12,r14}\n\t"
  "ldr sp, [%[next]]\n\t"
  : /* No Outputs */
  : [current] "r" (currentPCB), [next] "r" (nextPCB) /* Inputs */
  : /* 'No' Clobbers */);
  /* We are now on the other context */
  return 0;
}


