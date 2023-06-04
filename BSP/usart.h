#ifndef     USART_H
#define     USART_h

#include    <stdint.h>
#include    "stm32f7xx_hal.h"

#ifdef      USART_MODULE
    #define     USART_MODULE_EXTERN
#else
    #define     USART_MODULE_EXTERN      extern
#endif

#define USART_REC_LEN
#define RXBUFFERSIZE 128


USART_MODULE_EXTERN     uint8_t USART_RX_BUF[USART_REC_LEN];
USART_MODULE_EXTERN     uint16_t USART_RX_STA;
USART_MODULE_EXTERN     UART_HandleTypeDef UART1_Handler; 

USART_MODULE_EXTERN uint8_t aRxBuffer[RXBUFFERSIZE];

USART_MODULE_EXTERN void uart_init(uint32_t bound);
USART_MODULE_EXTERN void HAL_UART_MspInit(UART_HandleTypeDef *huart);

#endif