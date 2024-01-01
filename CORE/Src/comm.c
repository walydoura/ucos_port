#define COMM_MODULE
#include "comm.h"
#include <stdint.h>

OS_TCB gCommTaskTCB;
CPU_STK gCommTaskSTK[COMM_TASK_STK_SIZE];

void comm_task(void* p_arg)
{
	OS_ERR err;
	uint8_t i = 0;
	while (1)
	{
		if (i % 256 == 0)
			OSTimeDlyHMSM(0,0,0,500, OS_OPT_TIME_DLY, &err);
		i++;
	};


}