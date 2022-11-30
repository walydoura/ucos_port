#define     UASRT_MODULE
#include    "usart.h"

//uint8_t USART_RX_BUF[USART_REC_LEN];
uint16_t USART_RX_STA = 0;
uint8_t aRxBuffer[RXBUFFERSIZE];
UART_HandleTypeDef UART1_Handler; // UART句柄

static void UART1_IDLE_RxEventCallback(struct __UART_HandleTypeDef *huart, uint16_t Pos);

//初始化IO 串口1
// bound:波特率
void uart_init(uint32_t bound)
{
    // UART 初始化设置
    UART1_Handler.Instance = USART1;                    // USART1
    UART1_Handler.Init.BaudRate = bound;                //波特率
    UART1_Handler.Init.WordLength = UART_WORDLENGTH_8B; //字长为8位数据格式
    UART1_Handler.Init.StopBits = UART_STOPBITS_1;      //一个停止位
    UART1_Handler.Init.Parity = UART_PARITY_NONE;       //无奇偶校验位
    UART1_Handler.Init.HwFlowCtl = UART_HWCONTROL_NONE; //无硬件流控
    UART1_Handler.Init.Mode = UART_MODE_TX_RX;          //收发模式
    HAL_UART_Init(&UART1_Handler);                      // HAL_UART_Init()会使能UART1

    HAL_UART_RegisterRxEventCallback(&UART1_Handler, UART1_IDLE_RxEventCallback);
    //HAL_UART_Receive_IT(&UART1_Handler, (uint8_t *)aRxBuffer, RXBUFFERSIZE); //该函数会开启接收中断：标志位UART_IT_RXNE，并且设置接收缓冲以及接收缓冲接收最大数据量
    HAL_UARTEx_ReceiveToIdle_IT(&UART1_Handler, (uint8_t *)aRxBuffer, RXBUFFERSIZE);
}

// UART底层初始化，时钟使能，引脚配置，中断配置
//此函数会被HAL_UART_Init()调用
// huart:串口句柄

void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
    // GPIO端口设置
    GPIO_InitTypeDef GPIO_Initure;

    if (huart->Instance == USART1) //如果是串口1，进行串口1 MSP初始化
    {
        __HAL_RCC_GPIOA_CLK_ENABLE();  //使能GPIOA时钟
        __HAL_RCC_USART1_CLK_ENABLE(); //使能USART1时钟

        GPIO_Initure.Pin = GPIO_PIN_9;            // PA9
        GPIO_Initure.Mode = GPIO_MODE_AF_PP;      //复用推挽输出
        GPIO_Initure.Pull = GPIO_PULLUP;          //上拉
        GPIO_Initure.Speed = GPIO_SPEED_FAST;     //高速
        GPIO_Initure.Alternate = GPIO_AF7_USART1; //复用为USART1
        HAL_GPIO_Init(GPIOA, &GPIO_Initure);      //初始化PA9

        GPIO_Initure.Pin = GPIO_PIN_10;      // PA10
        HAL_GPIO_Init(GPIOA, &GPIO_Initure); //初始化PA10

        HAL_NVIC_EnableIRQ(USART1_IRQn);         //使能USART1中断通道
        HAL_NVIC_SetPriority(USART1_IRQn, 15, 0); //抢占优先级3，子优先级3

    }
}

void USART1_IRQHandler()
{
    HAL_UART_IRQHandler(&UART1_Handler);
}

void UART1_IDLE_RxEventCallback(struct __UART_HandleTypeDef *huart, uint16_t Pos)
{
    if (Pos == RXBUFFERSIZE) /*接收缓冲区满（收到了指定的字节数）*/
    {
        HAL_UART_Transmit_IT(huart, "FULL", 4);
    }
    else if (Pos < RXBUFFERSIZE) /*空闲字符中断（缓冲区未满）*/
    {
        HAL_UART_Transmit_IT(huart, "IDEL", 4);
    }
    
    HAL_UARTEx_ReceiveToIdle_IT(huart, aRxBuffer, RXBUFFERSIZE);
}
