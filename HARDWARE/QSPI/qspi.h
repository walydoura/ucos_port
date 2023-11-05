#ifndef __QSPI_H
#define __QSPI_H
#include "includes.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32开发板 
//QSPI 驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2016/7/18
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	 
 


uint8_t QSPI_Wait_Flag(uint32_t flag,uint8_t sta,uint32_t wtime);					//QSPI等待某个状态
uint8_t QSPI_Init(void);												//初始化QSPI
void QSPI_Send_CMD(uint8_t cmd,uint32_t addr,uint8_t mode,uint8_t dmcycle);			//QSPI发送命令
uint8_t QSPI_Receive(uint8_t* buf,uint32_t datalen);							//QSPI接收数据
uint8_t QSPI_Transmit(uint8_t* buf,uint32_t datalen);							//QSPI发送数据

#endif

