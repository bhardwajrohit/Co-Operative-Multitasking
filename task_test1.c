/*
 File        : task_test1.c
 Program     : INFO8110
 Programmer  : Rohit Bhardwaj
 Date        : 2016-11-30
 Description : This program works as a real co-operative multitasker
*/

//Header Files
#include <stdio.h>
#include <string.h>
#include "stm32f3xx_hal.h"
#include "task_executive1.h"
#include "stm32f3_discovery.h"
#include "common.h"

// Function     : Task1()
// Description  : This function simply prints something             
// Parameters   : void *data
// Returns      : None
void Task1(void *data)
{
   printf("Task 1 Starts\n");
   TaskSwitcher();
   printf("Task 1 Ends\n");
}

// Function     : Task2()
// Description  : This function simply prints something               
// Parameters   : void *data
// Returns      : None
void Task2(void *data)
{
   printf("Task 2 Starts\n");
   TaskSwitcher();
   printf("Task 2 Ends\n");
}

// Function     : Task3()
// Description  : This function simply prints something               
// Parameters   : void *data
// Returns      : None
void Task3(void *data)
{
   printf("Task 3 Starts\n");
   TaskSwitcher();
   printf("Task 3 Ends\n");
}

// Function     : Task4()
// Description  : This function simply prints something               
// Parameters   : void *data
// Returns      : None
void Task4(void *data)
{
   printf("Task 4 Starts\n");
   TaskSwitcher();
   printf("Task 4 Ends\n");
}

// Function     : CmdTaskinit()
// Description  : This command will intialize the task executive               
// Parameters   : int mode
// Returns      : None
void CmdTaskinit(int mode)
{
  printf("Intializing Task Executive\n");
  TaskInit();
}

// Function     : CmdTaskadd()
// Description  : This command will add all the tasks             
// Parameters   : int mode
// Returns      : None
void CmdTaskadd(int mode)
{
  printf("Adding tasks\n");
  TaskAdd(Task1, NULL);
  TaskAdd(Task2, NULL);
  TaskAdd(Task3, NULL);
  TaskAdd(Task4, NULL);
}

// Function     : CmdTaskswitcher()
// Description  : This command will call the task switcher               
// Parameters   : int mode
// Returns      : None
void CmdTaskswitcher(int mode)
{
 
   printf("Running Task Switcher\n");

   TaskSwitcher();

}

ADD_CMD("taskinit",CmdTaskinit,"               taskinit");

ADD_CMD("taskadd",CmdTaskadd,"               taskadd");

ADD_CMD("taskswitcher",CmdTaskswitcher,"                Taskswitcher");
