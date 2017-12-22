# ifndef TASK_EXECUTIVE_H
# define TASK_EXECUTIVE_H

int32_t TaskAdd(void (*f)(void *data), void *data);
int32_t TaskKill(int32_t id);

int32_t TaskSwitcher(void);
void Task1(void *data);
void Task2(void *data);
void CmdTaskTest(int mode);
void TaskInit(void);


# endif
