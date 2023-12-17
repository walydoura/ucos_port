#include "at24c02.h"

uint8_t AT24C02_ReadBytes(uint8_t addr, uint8_t len, uint8_t* recv_buf)
{
	LL_I2C_DisableAutoEndMode(I2C2);  //失能自动停止功能
	LL_I2C_SetMasterAddressingMode(I2C2, LL_I2C_ADDRESSING_MODE_7BIT);
	LL_I2C_SetSlaveAddr(I2C2, 0xA0); 
	LL_I2C_SetTransferRequest(I2C2, LL_I2C_REQUEST_WRITE);
	LL_I2C_SetTransferSize(I2C2, 1);

	while (LL_I2C_IsActiveFlag_BUSY(I2C2));
	LL_I2C_GenerateStartCondition(I2C2);
	while (!LL_I2C_IsActiveFlag_TXIS(I2C2));
	LL_I2C_TransmitData8(I2C2, addr);
	while (!LL_I2C_IsActiveFlag_TC(I2C2));

	LL_I2C_SetMasterAddressingMode(I2C2, LL_I2C_ADDRESSING_MODE_7BIT);
	LL_I2C_SetSlaveAddr(I2C2, 0xA0);
	LL_I2C_SetTransferRequest(I2C2, LL_I2C_REQUEST_READ);
	LL_I2C_SetTransferSize(I2C2, len);
	
	LL_I2C_GenerateStartCondition(I2C2);
	for (uint8_t i = 0; i < len; i++)
	{
		while (!LL_I2C_IsActiveFlag_RXNE(I2C2));
		recv_buf[i] = LL_I2C_ReceiveData8(I2C2);
		if (i == len)
		{
			while (!LL_I2C_IsActiveFlag_TC(I2C2));
			LL_I2C_GenerateStopCondition(I2C2);
		}
	}

	return 0;
}

uint8_t AT24C02_WriteBytes(uint8_t addr, uint8_t len, uint8_t* tx_buf)
{
	LL_I2C_DisableAutoEndMode(I2C2);  //失能自动停止功能
	LL_I2C_SetMasterAddressingMode(I2C2, LL_I2C_ADDRESSING_MODE_7BIT);
	LL_I2C_SetSlaveAddr(I2C2, 0xA0);
	LL_I2C_SetTransferRequest(I2C2, LL_I2C_REQUEST_WRITE);
	LL_I2C_SetTransferSize(I2C2, 2);	

	while (LL_I2C_IsActiveFlag_BUSY(I2C2));
	for (uint8_t i = 0; i < len; i++)
	{
		LL_I2C_GenerateStartCondition(I2C2);
		while (!LL_I2C_IsActiveFlag_TXIS(I2C2));
		LL_I2C_TransmitData8(I2C2, addr+i);
		while (!LL_I2C_IsActiveFlag_TXIS(I2C2));
		LL_I2C_TransmitData8(I2C2, tx_buf[i]);
		while (!LL_I2C_IsActiveFlag_TC(I2C2));
		LL_I2C_GenerateStopCondition(I2C2);
		LL_mDelay(5);
	}
	return 0;
}
