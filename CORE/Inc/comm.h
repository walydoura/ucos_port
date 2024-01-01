#ifndef COMM_H
#define COMM_h


#ifdef COMM_MODULE
#define COMM_EXTERN
#else
#define COMM_EXTERN   extern
#endif // COMM_MODULE

#include "os.h"

#define COMM_TASK_PRIO	5
#define COMM_TASK_STK_SIZE	128

COMM_EXTERN	OS_TCB gCommTaskTCB;
COMM_EXTERN	CPU_STK gCommTaskSTK[COMM_TASK_STK_SIZE];


COMM_EXTERN	void comm_task(void* p_arg);

#endif



