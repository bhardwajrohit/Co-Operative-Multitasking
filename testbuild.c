//Header Files
#include <stdio.h>
#include <string.h>
#include "stm32f3xx_hal.h"
#include "task_executive.h"
#include "common.h"


void Task1(void *data)
{
printf("Hello from Task 1\n");
}

void Task2(void *data)
{
printf("Hello from Task 2\n");
}

void CmdTaskTest(int mode)
{
    if(mode != CMD_INTERACTIVE) {
    return;
  }
  printf("Intializing Task Executive\n");
  TaskInit();

  printf("Adding tasks\n");
  TaskAdd(Task1, NULL);
  TaskAdd(Task2, NULL);
  
  printf("Running Task Switcher\n");

  for(int i = 0; i<25; i++)
  {
   TaskSwitcher();
  }
}

