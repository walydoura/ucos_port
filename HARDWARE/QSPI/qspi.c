#include "qspi.h"
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

//等待状态标志
//flag:需要等待的标志位
//sta:需要等待的状态
//wtime:等待时间
//返回值:0,等待成功.
//	     1,等待失败.
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

//初始化QSPI接口
//返回值:0,成功;
//       1,失败;
uint8_t QSPI_Init(void)
{
	uint32_t tempreg=0;
	//RCC->AHB1ENR|=1<<1;    		//使能PORTB时钟	   
	//RCC->AHB1ENR|=1<<5;    		//使能PORTF时钟	   
	//RCC->AHB3ENR|=1<<1;   		//QSPI时钟使能

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

	if(QSPI_Wait_Flag(QUADSPI_SR_BUSY,0,0XFFFF)==0)//等待BUSY空闲
	{
		tempreg=(3-1)<< QUADSPI_CR_PRESCALER_Pos;		//设置QSPI时钟为AHB时钟的1/3,即216M/3=72Mhz,13.8ns
		tempreg|=(4-1)<< QUADSPI_CR_FTHRES_Pos;		//设置FIFO阈值为4个字节(最大为31,表示32个字节)
		tempreg|=0<< QUADSPI_CR_FSEL_Pos;			//选择FLASH1
		tempreg|=0<< QUADSPI_CR_DFM_Pos;			//禁止双闪存模式
		tempreg|=1<< QUADSPI_CR_SSHIFT_Pos;			//采样移位半个周期(DDR模式下,必须设置为0)
		QUADSPI->CR=tempreg;	//设置CR寄存器
		tempreg=(25-1)<<QUADSPI_DCR_FSIZE_Pos;		//设置FLASH大小为2^25=32MB
		tempreg|=(4-1)<<QUADSPI_DCR_CSHT_Pos;		//片选高电平时间为4个时钟(13.8*4=55.2ns),即手册里面的tSHSL参数
		tempreg|=1<<QUADSPI_DCR_CKMODE_Pos;			//Mode3,空闲时CLK为高电平
		QUADSPI->DCR=tempreg;	//设置DCR寄存器
		QUADSPI->CR|=1<<QUADSPI_CR_EN_Pos;		//使能QSPI
	}else return 1;
	return 0;
}

//QSPI发送命令
//cmd:要发送的指令
//addr:发送到的目的地址
//mode:模式,详细位定义如下:
//	mode[1:0]:指令模式;00,无指令;01,单线传输指令;10,双线传输指令;11,四线传输指令.
//	mode[3:2]:地址模式;00,无地址;01,单线传输地址;10,双线传输地址;11,四线传输地址.
//	mode[5:4]:地址长度;00,8位地址;01,16位地址;10,24位地址;11,32位地址.
//	mode[7:6]:数据模式;00,无数据;01,单线传输数据;10,双线传输数据;11,四线传输数据.
//dmcycle:空指令周期数
void QSPI_Send_CMD(uint8_t cmd,uint32_t addr,uint8_t mode,uint8_t dmcycle)
{
	uint32_t tempreg=0;	
	uint8_t status;
	if(QSPI_Wait_Flag(1<<5,0,0XFFFF)==0)	//等待BUSY空闲
	{
		tempreg=0<<31;						//禁止DDR模式
		tempreg|=0<<28;						//每次都发送指令
		tempreg|=0<<26;						//间接写模式
		tempreg|=((uint32_t)mode>>6)<<24;		//设置数据模式
		tempreg|=(uint32_t)dmcycle<<18;			//设置空指令周期数
		tempreg|=((uint32_t)(mode>>4)&0X03)<<12;	//设置地址长度
		tempreg|=((uint32_t)(mode>>2)&0X03)<<10;	//设置地址模式
		tempreg|=((uint32_t)(mode>>0)&0X03)<<8;	//设置指令模式
		tempreg|=cmd;						//设置指令
		QUADSPI->CCR=tempreg;				//设置CCR寄存器
		if(mode&0X0C)						//有指令+地址要发送
		{
			QUADSPI->AR=addr;				//设置地址寄存器
		} 
		if((mode&0XC0)==0)					//无数据传输,等待指令发送完成
		{
			status=QSPI_Wait_Flag(1<<1,1,0XFFFF);//等待TCF,即传输完成
			if(status==0)
			{
				QUADSPI->FCR|=1<<1;			//清除TCF标志位 
			}
		}
	}	
}

//QSPI接收指定长度的数据
//buf:接收数据缓冲区首地址
//datalen:要传输的数据长度
//返回值:0,正常
//    其他,错误代码
uint8_t QSPI_Receive(uint8_t* buf,uint32_t datalen)
{
	uint32_t tempreg=QUADSPI->CCR;
	uint32_t addrreg=QUADSPI->AR; 	
	uint8_t status;
	volatile uint32_t *data_reg=&QUADSPI->DR;
	QUADSPI->DLR=datalen-1;					//设置数据传输长度
	tempreg&=~(3<<26);						//清除FMODE原来的设置
	tempreg|=1<<26;							//设置FMODE为间接读取模式
	QUADSPI->FCR|=1<<1;						//清除TCF标志位
	QUADSPI->CCR=tempreg;					//回写CCR寄存器
	QUADSPI->AR=addrreg;					//回写AR寄存器,触发传输
	while(datalen)
	{
		status=QSPI_Wait_Flag(3<<1,1,0XFFFF);//等到FTF和TCF,即接收到了数据
		if(status==0)						//等待成功
		{
			*buf++=*(volatile uint8_t *)data_reg;
			datalen--;
		}else break;
	}
	if(status==0)
	{
		QUADSPI->CR|=1<<2;							//终止传输 
		status=QSPI_Wait_Flag(1<<1,1,0XFFFF);		//等待TCF,即数据传输完成
		if(status==0)
		{
			QUADSPI->FCR|=1<<1;						//清除TCF标志位 
			status=QSPI_Wait_Flag(1<<5,0,0XFFFF);	//等待BUSY位清零
		}
	}
	return status;
} 

//QSPI发送指定长度的数据
//buf:发送数据缓冲区首地址
//datalen:要传输的数据长度
//返回值:0,正常
//    其他,错误代码
uint8_t QSPI_Transmit(uint8_t* buf,uint32_t datalen)
{
	uint32_t tempreg=QUADSPI->CCR;
	uint32_t addrreg=QUADSPI->AR; 
	uint8_t status;
	volatile uint32_t *data_reg=&QUADSPI->DR;
	QUADSPI->DLR=datalen-1;					//设置数据传输长度
	tempreg&=~(3<<26);						//清除FMODE原来的设置
	tempreg|=0<<26;							//设置FMODE为间接写入模式
	QUADSPI->FCR|=1<<1;						//清除TCF标志位
	QUADSPI->CCR=tempreg;					//回写CCR寄存器 
	while(datalen)
	{
		status=QSPI_Wait_Flag(1<<2,1,0XFFFF);//等到FTF
		if(status!=0)						//等待成功
		{
			break;
		}
		*(volatile uint8_t *)data_reg=*buf++;
		datalen--;
	}
	if(status==0)
	{
		QUADSPI->CR|=1<<2;							//终止传输 
		status=QSPI_Wait_Flag(1<<1,1,0XFFFF);		//等待TCF,即数据传输完成
		if(status==0)
		{
			QUADSPI->FCR|=1<<1;						//清除TCF标志位 
			status=QSPI_Wait_Flag(1<<5,0,0XFFFF);	//等待BUSY位清零
		}
	}
	return status;
}




















