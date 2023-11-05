#ifndef __QSPI_H
#define __QSPI_H
#include "includes.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32������ 
//QSPI ��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2016/7/18
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	 
 


uint8_t QSPI_Wait_Flag(uint32_t flag,uint8_t sta,uint32_t wtime);					//QSPI�ȴ�ĳ��״̬
uint8_t QSPI_Init(void);												//��ʼ��QSPI
void QSPI_Send_CMD(uint8_t cmd,uint32_t addr,uint8_t mode,uint8_t dmcycle);			//QSPI��������
uint8_t QSPI_Receive(uint8_t* buf,uint32_t datalen);							//QSPI��������
uint8_t QSPI_Transmit(uint8_t* buf,uint32_t datalen);							//QSPI��������

#endif

