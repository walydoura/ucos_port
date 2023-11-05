#include "qspi.h"
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

//�ȴ�״̬��־
//flag:��Ҫ�ȴ��ı�־λ
//sta:��Ҫ�ȴ���״̬
//wtime:�ȴ�ʱ��
//����ֵ:0,�ȴ��ɹ�.
//	     1,�ȴ�ʧ��.
uint8_t QSPI_Wait_Flag(uint32_t flag,uint8_t sta,uint32_t wtime)
{
	uint8_t flagsta=0;
	while(wtime)
	{
		flagsta=(QUADSPI->SR&flag)?1:0; 
		if(flagsta==sta)break;
		wtime--;
	}
	if(wtime)return 0;
	else return 1;
}

//��ʼ��QSPI�ӿ�
//����ֵ:0,�ɹ�;
//       1,ʧ��;
uint8_t QSPI_Init(void)
{
	uint32_t tempreg=0;
	//RCC->AHB1ENR|=1<<1;    		//ʹ��PORTBʱ��	   
	//RCC->AHB1ENR|=1<<5;    		//ʹ��PORTFʱ��	   
	//RCC->AHB3ENR|=1<<1;   		//QSPIʱ��ʹ��

	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOF_CLK_ENABLE();
	__HAL_RCC_QSPI_CLK_ENABLE();

	LL_GPIO_InitTypeDef t_gpio;
	t_gpio.Pin = LL_GPIO_PIN_2;
	t_gpio.Mode = LL_GPIO_MODE_ALTERNATE;
	t_gpio.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	t_gpio.Speed = LL_GPIO_SPEED_FREQ_HIGH;
	t_gpio.Pull = LL_GPIO_PULL_UP;
	t_gpio.Alternate = LL_GPIO_AF_9;
	LL_GPIO_Init(GPIOB, &t_gpio);

	t_gpio.Pin = LL_GPIO_PIN_6;
	t_gpio.Alternate = LL_GPIO_AF_10;
	LL_GPIO_Init(GPIOB, &t_gpio);

	t_gpio.Pin = LL_GPIO_PIN_6 | LL_GPIO_PIN_7;
	t_gpio.Alternate = LL_GPIO_AF_9;
	LL_GPIO_Init(GPIOF, &t_gpio);

	t_gpio.Pin = LL_GPIO_PIN_8 | LL_GPIO_PIN_9;
	t_gpio.Alternate = LL_GPIO_AF_10;
	LL_GPIO_Init(GPIOF, &t_gpio);


	__HAL_RCC_QSPI_FORCE_RESET();
	__HAL_RCC_QSPI_RELEASE_RESET();

	if(QSPI_Wait_Flag(QUADSPI_SR_BUSY,0,0XFFFF)==0)//�ȴ�BUSY����
	{
		tempreg=(3-1)<< QUADSPI_CR_PRESCALER_Pos;		//����QSPIʱ��ΪAHBʱ�ӵ�1/3,��216M/3=72Mhz,13.8ns
		tempreg|=(4-1)<< QUADSPI_CR_FTHRES_Pos;		//����FIFO��ֵΪ4���ֽ�(���Ϊ31,��ʾ32���ֽ�)
		tempreg|=0<< QUADSPI_CR_FSEL_Pos;			//ѡ��FLASH1
		tempreg|=0<< QUADSPI_CR_DFM_Pos;			//��ֹ˫����ģʽ
		tempreg|=1<< QUADSPI_CR_SSHIFT_Pos;			//������λ�������(DDRģʽ��,��������Ϊ0)
		QUADSPI->CR=tempreg;	//����CR�Ĵ���
		tempreg=(25-1)<<QUADSPI_DCR_FSIZE_Pos;		//����FLASH��СΪ2^25=32MB
		tempreg|=(4-1)<<QUADSPI_DCR_CSHT_Pos;		//Ƭѡ�ߵ�ƽʱ��Ϊ4��ʱ��(13.8*4=55.2ns),���ֲ������tSHSL����
		tempreg|=1<<QUADSPI_DCR_CKMODE_Pos;			//Mode3,����ʱCLKΪ�ߵ�ƽ
		QUADSPI->DCR=tempreg;	//����DCR�Ĵ���
		QUADSPI->CR|=1<<QUADSPI_CR_EN_Pos;		//ʹ��QSPI
	}else return 1;
	return 0;
}

//QSPI��������
//cmd:Ҫ���͵�ָ��
//addr:���͵���Ŀ�ĵ�ַ
//mode:ģʽ,��ϸλ��������:
//	mode[1:0]:ָ��ģʽ;00,��ָ��;01,���ߴ���ָ��;10,˫�ߴ���ָ��;11,���ߴ���ָ��.
//	mode[3:2]:��ַģʽ;00,�޵�ַ;01,���ߴ����ַ;10,˫�ߴ����ַ;11,���ߴ����ַ.
//	mode[5:4]:��ַ����;00,8λ��ַ;01,16λ��ַ;10,24λ��ַ;11,32λ��ַ.
//	mode[7:6]:����ģʽ;00,������;01,���ߴ�������;10,˫�ߴ�������;11,���ߴ�������.
//dmcycle:��ָ��������
void QSPI_Send_CMD(uint8_t cmd,uint32_t addr,uint8_t mode,uint8_t dmcycle)
{
	uint32_t tempreg=0;	
	uint8_t status;
	if(QSPI_Wait_Flag(1<<5,0,0XFFFF)==0)	//�ȴ�BUSY����
	{
		tempreg=0<<31;						//��ֹDDRģʽ
		tempreg|=0<<28;						//ÿ�ζ�����ָ��
		tempreg|=0<<26;						//���дģʽ
		tempreg|=((uint32_t)mode>>6)<<24;		//��������ģʽ
		tempreg|=(uint32_t)dmcycle<<18;			//���ÿ�ָ��������
		tempreg|=((uint32_t)(mode>>4)&0X03)<<12;	//���õ�ַ����
		tempreg|=((uint32_t)(mode>>2)&0X03)<<10;	//���õ�ַģʽ
		tempreg|=((uint32_t)(mode>>0)&0X03)<<8;	//����ָ��ģʽ
		tempreg|=cmd;						//����ָ��
		QUADSPI->CCR=tempreg;				//����CCR�Ĵ���
		if(mode&0X0C)						//��ָ��+��ַҪ����
		{
			QUADSPI->AR=addr;				//���õ�ַ�Ĵ���
		} 
		if((mode&0XC0)==0)					//�����ݴ���,�ȴ�ָ������
		{
			status=QSPI_Wait_Flag(1<<1,1,0XFFFF);//�ȴ�TCF,���������
			if(status==0)
			{
				QUADSPI->FCR|=1<<1;			//���TCF��־λ 
			}
		}
	}	
}

//QSPI����ָ�����ȵ�����
//buf:�������ݻ������׵�ַ
//datalen:Ҫ��������ݳ���
//����ֵ:0,����
//    ����,�������
uint8_t QSPI_Receive(uint8_t* buf,uint32_t datalen)
{
	uint32_t tempreg=QUADSPI->CCR;
	uint32_t addrreg=QUADSPI->AR; 	
	uint8_t status;
	volatile uint32_t *data_reg=&QUADSPI->DR;
	QUADSPI->DLR=datalen-1;					//�������ݴ��䳤��
	tempreg&=~(3<<26);						//���FMODEԭ��������
	tempreg|=1<<26;							//����FMODEΪ��Ӷ�ȡģʽ
	QUADSPI->FCR|=1<<1;						//���TCF��־λ
	QUADSPI->CCR=tempreg;					//��дCCR�Ĵ���
	QUADSPI->AR=addrreg;					//��дAR�Ĵ���,��������
	while(datalen)
	{
		status=QSPI_Wait_Flag(3<<1,1,0XFFFF);//�ȵ�FTF��TCF,�����յ�������
		if(status==0)						//�ȴ��ɹ�
		{
			*buf++=*(volatile uint8_t *)data_reg;
			datalen--;
		}else break;
	}
	if(status==0)
	{
		QUADSPI->CR|=1<<2;							//��ֹ���� 
		status=QSPI_Wait_Flag(1<<1,1,0XFFFF);		//�ȴ�TCF,�����ݴ������
		if(status==0)
		{
			QUADSPI->FCR|=1<<1;						//���TCF��־λ 
			status=QSPI_Wait_Flag(1<<5,0,0XFFFF);	//�ȴ�BUSYλ����
		}
	}
	return status;
} 

//QSPI����ָ�����ȵ�����
//buf:�������ݻ������׵�ַ
//datalen:Ҫ��������ݳ���
//����ֵ:0,����
//    ����,�������
uint8_t QSPI_Transmit(uint8_t* buf,uint32_t datalen)
{
	uint32_t tempreg=QUADSPI->CCR;
	uint32_t addrreg=QUADSPI->AR; 
	uint8_t status;
	volatile uint32_t *data_reg=&QUADSPI->DR;
	QUADSPI->DLR=datalen-1;					//�������ݴ��䳤��
	tempreg&=~(3<<26);						//���FMODEԭ��������
	tempreg|=0<<26;							//����FMODEΪ���д��ģʽ
	QUADSPI->FCR|=1<<1;						//���TCF��־λ
	QUADSPI->CCR=tempreg;					//��дCCR�Ĵ��� 
	while(datalen)
	{
		status=QSPI_Wait_Flag(1<<2,1,0XFFFF);//�ȵ�FTF
		if(status!=0)						//�ȴ��ɹ�
		{
			break;
		}
		*(volatile uint8_t *)data_reg=*buf++;
		datalen--;
	}
	if(status==0)
	{
		QUADSPI->CR|=1<<2;							//��ֹ���� 
		status=QSPI_Wait_Flag(1<<1,1,0XFFFF);		//�ȴ�TCF,�����ݴ������
		if(status==0)
		{
			QUADSPI->FCR|=1<<1;						//���TCF��־λ 
			status=QSPI_Wait_Flag(1<<5,0,0XFFFF);	//�ȴ�BUSYλ����
		}
	}
	return status;
}




















