#include "stm8l15x.h" //STM8L051/151等系列共用库函数
#include "24c0x.h"
#include "bq28z610.h"
#include "comm.h"

// 定义LED端口
#define LED1_PORT GPIOD
#define LED1_PINS GPIO_Pin_0
#define LED2_PORT GPIOC
#define LED2_PINS GPIO_Pin_4
#define LED3_PORT GPIOB
#define LED3_PINS GPIO_Pin_2

unsigned char WBuf[] = {"STM8L IIC读写24C0X!\r\n"};
#define BufferSize 20
unsigned char RBuf[BufferSize];

uint8_t uart_data = 0;
uint8_t ifGetData = 0;

/*******************************************************************************
****函数说明：主函数
****入口参数:无
****出口参数:无
****函数备注:LED以一定频率闪烁
********************************************************************************/
void main(void)
{
    CLK_SYSCLKDivConfig(CLK_SYSCLKDiv_8);
    GPIO_Init(LED1_PORT, LED1_PINS, GPIO_Mode_Out_PP_Low_Slow);                                                      // 初始化LED端口
    CLK_PeripheralClockConfig(CLK_Peripheral_USART1, ENABLE);                                                        // 开启USART时钟
    USART_Init(USART1, 9600, USART_WordLength_8b, USART_StopBits_1, USART_Parity_No, USART_Mode_Tx | USART_Mode_Rx); // 设置USART参数9600，8N1，接收/发送
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);                                                                   // 使能接收中断
    USART_Cmd(USART1, ENABLE);                                                                                       // 使能USART
    enableInterrupts();

    InitIIC();
    // EEPROM_WriteData(WBuf, EEPROM_BASE_ADDRESS, BufferSize);
    // EEPROM_ReadData(RBuf, EEPROM_BASE_ADDRESS, BufferSize);

    while (1)
    {

        // i2c_test();

        // FETControl();
        // get_manufacturing_status();
        Delay(5000);

        if (ifGetData)
        {
            handle_comm(uart_data);
            ifGetData = 0;
        }

        // get_voltage();
        // Delay(500);

        // get_current();
        // Delay(500);

        // get_cell_vol();
        // Delay(500);

        GPIO_ToggleBits(LED1_PORT, LED1_PINS); // 翻转LED端口
    }
}

#ifdef USE_FULL_ASSERT

/**
 * @brief  Reports the name of the source file and the source line number
 *   where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line)
{
    /* User can add his own implementation to report the file name and line number,
       ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

    /* Infinite loop */
    while (1)
    {
    }
}
#endif

/**
 * @}
 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
