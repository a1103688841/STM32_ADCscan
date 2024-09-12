#include "my_define.h"

#include "../Core/Inc/usart.h"

// 重定向c库函数printf到串口USARTx，重定向后可使用printf函数
int fputc(int ch, FILE* f)
{
    /* 发送一个字节数据到串口USARTx */
    HAL_UART_Transmit(&huart1, (uint8_t*)&ch, 1, 0xFFFF);
    return (ch);
}

// 重定向c库函数scanf到串口USARTx，重写向后可使用scanf、getchar等函�?
int fgetc(FILE* f)
{
    int ch;
    /* 等待串口输入数据 */
    while (__HAL_UART_GET_FLAG(&huart1, UART_FLAG_RXNE) == RESET)
        ;
    HAL_UART_Receive(&huart1, (uint8_t*)&ch, 1, 0xFFFF);
    return (ch);
}