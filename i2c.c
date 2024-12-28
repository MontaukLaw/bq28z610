#include "i2c.h"

#define WAIT_OVER_TIME           \
    over_time_counter++;         \
    if (over_time_counter > 200) \
    {                            \
        break;                   \
    }

void send_command_with_sub(uint8_t reg, uint16_t sub_command)
{

    uint32_t over_time_counter = 0;
    /* 等待空闲 */
    while (I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY))
    {
        over_time_counter++;
        if (over_time_counter > 200)
        {
            break;
        }
    }

    /* 发起始位 */
    I2C_GenerateSTART(I2C1, ENABLE);
    over_time_counter = 0;
    /* 测试EV5 ，检测从器件返回一个应答信号*/
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT))
    {
        WAIT_OVER_TIME
    }

    /* 设置I2C从器件地址，I2C主设备为写模式*/
    I2C_Send7bitAddress(I2C1, BQ28Z610_I2C_ADDR, I2C_Direction_Transmitter);
    over_time_counter = 0;
    /* 测试EV6 ，检测从器件返回一个应答信号*/
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))
    {
        WAIT_OVER_TIME
    }

    /* 要写 0x3E  */
    I2C_SendData(I2C1, reg);
    /* 测试EV6 ，检测从器件返回一个应答信号*/
    over_time_counter = 0;
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
    {
        WAIT_OVER_TIME
    }

    I2C_SendData(I2C1, (uint8_t)(sub_command & 0x00ff));
    /* 测试EV8 ，检测从器件返回一个应答信号*/
    over_time_counter = 0;
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
    {
        WAIT_OVER_TIME
    }

    I2C_SendData(I2C1, (uint8_t)((sub_command & 0xFF00) >> 8));
    /* 测试EV8 ，检测从器件返回一个应答信号*/
    over_time_counter = 0;
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
    {
        WAIT_OVER_TIME
    }

    /* 发结束位*/
    I2C_GenerateSTOP(I2C1, ENABLE);
}

void send_command(uint8_t command)
{

    uint32_t over_time_counter = 0;

    /* 等待空闲 */
    while (I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY))
    {
        WAIT_OVER_TIME
    }

    /* 发起始位 */
    I2C_GenerateSTART(I2C1, ENABLE);
    /* 测试EV5 ，检测从器件返回一个应答信号*/
    over_time_counter = 0;
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT))
    {
        WAIT_OVER_TIME
    }

    /* 设置I2C从器件地址，I2C主设备为写模式*/
    I2C_Send7bitAddress(I2C1, BQ28Z610_I2C_ADDR, I2C_Direction_Transmitter);
    over_time_counter = 0;
    /* 测试EV6 ，检测从器件返回一个应答信号*/
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))
    {
        WAIT_OVER_TIME
    }

    /* 要写 0x3E  */
    I2C_SendData(I2C1, command);
    /* 测试EV6 ，检测从器件返回一个应答信号*/
    over_time_counter = 0;
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
    {
        WAIT_OVER_TIME
    }

    /* 发结束位*/
    I2C_GenerateSTOP(I2C1, ENABLE);
}

int requestBytes(uint8_t *buffer, int len)
{
    // BQ28Z610_I2C_ADDR
    int actual = 0;
    uint32_t over_time_counter = 0;

    /* 等待空闲 */
    while (I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY))
    {
        over_time_counter++;
        if (over_time_counter > 200)
        {
            return -1;
        }
    }

    /* 发起始位 */
    I2C_GenerateSTART(I2C1, ENABLE);
    over_time_counter = 0;
    /* 测试EV5 ，检测从器件返回一个应答信号*/
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT))
    {
        over_time_counter++;
        if (over_time_counter > 200)
        {
            I2C_GenerateSTOP(I2C1, ENABLE); // 生成停止条件以释放总线
            return -1;
        }
    }

    /* 设置I2C从器件地址，I2C主设备为读模式*/
    I2C_Send7bitAddress(I2C1, BQ28Z610_I2C_ADDR, I2C_Direction_Receiver);
    /* 测试EV6 ，检测从器件返回一个应答信号*/
    over_time_counter = 0;
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))
    {
        over_time_counter++;
        if (over_time_counter > 200)
        {
            I2C_GenerateSTOP(I2C1, ENABLE); // 生成停止条件以释放总线
            return -1;
        }
    }

    /*不断在从从设备读取数据 */
    while (len)
    {

        /* 读到最后一个字节*/
        if (len == 1)
        {
            /* 不需要应答*/
            I2C_AcknowledgeConfig(I2C1, DISABLE);
            /* 发结束位*/
            I2C_GenerateSTOP(I2C1, ENABLE);
        }
        else
        {
            /* 需要应答*/
            I2C_AcknowledgeConfig(I2C1, ENABLE);
        }

        over_time_counter = 0;
        while (!I2C_GetFlagStatus(I2C1, I2C_FLAG_RXNE))
        {
            over_time_counter++;
            if (over_time_counter > 200)
            {
                return -1; // 超时
            }
        }

        *buffer = I2C_ReceiveData(I2C1);
        /* 指针指向下个存放字节的地址*/
        buffer++;
        len--;

        actual++;
    }

    return actual;
}

int request2Bytes(uint8_t *buffer, int len)
{
    uint32_t over_time_counter = 0;

    /* 等待总线空闲 */
    while (I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY))
    {
        over_time_counter++;
        if (over_time_counter > 200)
        {
            return -1; // 超时
        }
    }

    /* 发起始位 */
    I2C_GenerateSTART(I2C1, ENABLE);

    over_time_counter = 0;
    /* 等待 EV5: 起始条件已生成，进入主模式 */
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT))
    {
        over_time_counter++;
        if (over_time_counter > 200)
        {
            I2C_GenerateSTOP(I2C1, ENABLE); // 生成停止条件以释放总线
            return -1;                      // 超时
        }
    }

    /* 发送从设备地址 + 读模式 */
    I2C_Send7bitAddress(I2C1, BQ28Z610_I2C_ADDR, I2C_Direction_Receiver);

    over_time_counter = 0;
    /* 等待 EV6: 从设备已应答，进入接收模式 */
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))
    {
        over_time_counter++;
        if (over_time_counter > 200)
        {
            I2C_GenerateSTOP(I2C1, ENABLE); // 生成停止条件以释放总线
            return -1;                      // 超时
        }
    }

    /* 配置 ACK 使能以接收第一个字节 */
    I2C_AcknowledgeConfig(I2C1, ENABLE);

    /* 读取第一个字节 */
    over_time_counter = 0;
    while (!I2C_GetFlagStatus(I2C1, I2C_FLAG_RXNE))
    {
        over_time_counter++;
        if (over_time_counter > 200)
        {
            I2C_GenerateSTOP(I2C1, ENABLE); // 生成停止条件以释放总线
            return -1;                      // 超时
        }
    }
    buffer[0] = I2C_ReceiveData(I2C1);

    /* 在接收最后一个字节前禁用 ACK */
    I2C_AcknowledgeConfig(I2C1, DISABLE);

    /* 生成停止条件，确保硬件不再请求额外的数据 */
    I2C_GenerateSTOP(I2C1, ENABLE);

    /* 读取第二个字节 */
    over_time_counter = 0;
    while (!I2C_GetFlagStatus(I2C1, I2C_FLAG_RXNE))
    {
        over_time_counter++;
        if (over_time_counter > 200)
        {
            return -1; // 超时
        }
    }
    buffer[1] = I2C_ReceiveData(I2C1);

    return len; // 成功接收
}

uint16_t requestWord(void)
{
    uint8_t buf[] = {0, 0};
    requestBytes(buf, sizeof(buf));
    return composeWord(buf, 0, 1);
}
