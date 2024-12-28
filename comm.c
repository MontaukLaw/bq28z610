#include "comm.h"

void send_uart_word(uint16_t data)
{
    USART_SendData8(USART1, data >> 8);
    while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET)
        ;

    USART_SendData8(USART1, data);
    while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET)
        ;
}

void send_uart_dword(uint32_t data)
{
    USART_SendData8(USART1, data >> 24);
    while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET)
        ;

    USART_SendData8(USART1, data >> 16);
    while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET)
        ;

    USART_SendData8(USART1, data >> 8);
    while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET)
        ;

    USART_SendData8(USART1, data);
    while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET)
        ;
}

void handle_comm(uint8_t data)
{
    uint16_t word_result = 0;
    uint32_t dword_result = 0;

    switch (data)
    {
        // 获取电池电压
    case 0x01:
        // 回复2个字节的电压值, 实际值需要乘以0.001
        word_result = get_voltage();

        // 串口回复
        send_uart_word(word_result);

        break;

        // 获取电池电流
    case 0x02:
        word_result = get_current();
        send_uart_word(word_result);
        break;

        // 获取电芯1/2电压
    case 0x03:
        dword_result = get_cell_vol();
        send_uart_dword(dword_result);
        break;

        // 获取温度
    case 0x04:
        // 回复2个字节的温度值, 实际值需要乘以0.1
        word_result = get_temperature();
        send_uart_word(word_result);
        break;

        // 获取充电, 放电, AFE开关状态
        // 12.2.33 AltManufacturerAccess()
        // FET_EN (Bit 4): All FET Action
        // DSG_TEST (Bit 2): Discharge FET Test
        // CHG_TEST (Bit 1): Charge FET Test
    case 0x05:
        word_result = get_manufacturing_status();
        send_uart_word(word_result);
        break;

        // AFE开关控制
    case 0x0a:
        FETControl();
        break;

        // 放电FET控制
    case 0x0b:
        ChargeFET();
        break;

    case 0x0c:
        DischargeFET();
        break;

    default:
        break;
    }
}